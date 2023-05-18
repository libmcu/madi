/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pnet/iface_usb.h"
#include "pusb/usbd.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "netif/etharp.h"
#include "lwip/snmp.h"
#include "lwip/esp_pbuf_ref.h"
#include "dhcpserver/dhcpserver.h"

#include "tusb.h"
#include "libmcu/board.h"

#define MTU		1514U

uint8_t tud_network_mac_address[6];

static struct netif *usb_netif;

struct netstack_cfg_wrapper {
	err_t (*init_fn)(struct netif *netif);
	void (*input_fn)(void *netif, void *buffer, size_t len, void *eb);
};

static int convert_hexstr_to_number(uint8_t *buf, size_t bufsize,
		const char *hexstr, size_t hexstr_len)
{
	size_t o = 0;

	for (size_t i = 0; i < hexstr_len && o < bufsize; i++) {
		char c = hexstr[i];
		char base = '0';

		if (c >= '0' && c <= '9') {
		} else if (c >= 'A' && c <= 'F') {
			base = 'A' - 0xA;
		} else if (c >= 'a' && c <= 'f') {
			base = 'a' - 0xa;
		} else {
			break;
		}

		if (i & 1) {
			buf[o] <<= 4;
			buf[o] |= c - base;
			o++;
		} else {
			buf[o] = c - base;
		}
	}

	return (int)o;
}

static void set_mac_address(void)
{
	if (tud_network_mac_address[0] || tud_network_mac_address[1] ||
			tud_network_mac_address[2] ||
			tud_network_mac_address[3] ||
			tud_network_mac_address[4] ||
			tud_network_mac_address[5]) {
		return;
	}

	const char *serial = board_get_serial_number_string();
	convert_hexstr_to_number(tud_network_mac_address,
			sizeof(tud_network_mac_address),
			serial, strlen(serial));
	tud_network_mac_address[0] |= 0x02U; /* locally administered */
	tud_network_mac_address[0] &= (uint8_t)~0x01U; /* unicast */
}

/* This function should never be called because esp wrapper layer not used. */
static void usb_driver_free_rx_buffer(void *h, void* buffer)
{
	(void)h;
	if (buffer) {
		free(buffer);
	}
}

static int on_tx(const void *p, void *q)
{
	const struct pbuf *pbuf = (const struct pbuf *)p;
	return (int)pbuf_copy_partial(pbuf, q, pbuf->tot_len, 0);
}

static esp_err_t tx_usb(void *h, void *buffer, size_t len)
{
	(void)h;
	usbd_cdc_net_send(buffer, len, on_tx);
	return ESP_OK;
}

static esp_err_t tx_usb_pre(void *h, void *buffer, size_t len,
		void *netstack_buf)
{
	(void)netstack_buf;
	return tx_usb(h, buffer, len);
}

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
	(void)netif;
	return usbd_cdc_net_send(p, p->tot_len, on_tx) == ERR_OK?
		ERR_OK : ERR_USE;
}

static err_t ip4_output_fn(struct netif *netif,
		struct pbuf *p, const ip4_addr_t *addr)
{
	return etharp_output(netif, p, addr);
}

static err_t init_cb(struct netif *netif)
{
	set_mac_address();
	netif->hwaddr_len = sizeof(tud_network_mac_address);
	memcpy(netif->hwaddr, tud_network_mac_address, netif->hwaddr_len);
	netif->hwaddr[5] ^= 0x01; /* to distinguish from the host's mac address. */

	LWIP_ASSERT("netif != NULL", (netif != NULL));
	netif->mtu = MTU;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
		NETIF_FLAG_LINK_UP | NETIF_FLAG_UP | NETIF_FLAG_IGMP;
	netif->state = NULL;
	netif->name[0] = 'e';
	netif->name[1] = 'n';
	netif->linkoutput = linkoutput_fn;
	netif->output = ip4_output_fn;

	return ERR_OK;
}

static int received(const uint8_t *src, uint16_t size)
{
	if (size) {
		struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

		if (p) {
			memcpy(p->payload, src, size);
			if (usb_netif->input(p, usb_netif) != ERR_OK) {
				pbuf_free(p);
			}
		}
	}

	return size;
}

/* This function should never be called because input goes through
 * tud_network_recv_cb() directly. */
static void rx_cb(void *h, void *buffer, size_t len, void *l2_buff)
{
	(void)h;
	(void)l2_buff;
	received(buffer, len);
}

struct netif *netif_create_usb_esp32(const struct netif_param *param)
{
	ESP_ERROR_CHECK(esp_netif_init()); /* The function is reentrant-safe. */

	static const struct netstack_cfg_wrapper netstack_cfg = {
		.init_fn = init_cb,
		.input_fn = rx_cb,
	};

	static esp_netif_driver_ifconfig_t usbdrv_cfg = {
		.driver_free_rx_buffer = usb_driver_free_rx_buffer,
		.transmit = tx_usb,
		.transmit_wrap = tx_usb_pre,
		.handle = "USB",
	};

	const esp_netif_inherent_config_t net_cfg = {
		.flags = ESP_NETIF_DHCP_SERVER | ESP_NETIF_FLAG_GARP |
			ESP_NETIF_FLAG_EVENT_IP_MODIFIED,
		.get_ip_event = IP_EVENT_STA_GOT_IP,
		.lost_ip_event = IP_EVENT_STA_LOST_IP,
		.if_key = "USBkey",
		.if_desc = "USBNetif" 
	};

	esp_netif_config_t cfg = {
		.base = &net_cfg,
		.driver = &usbdrv_cfg,
		.stack = (const esp_netif_netstack_config_t*)&netstack_cfg,
	};

	esp_netif_t *netif = esp_netif_new(&cfg);
	esp_netif_dhcps_stop(netif);
	esp_netif_set_ip_info(netif, (esp_netif_ip_info_t *)param);

	esp_netif_action_start(netif, NULL, 0, NULL);
	extern esp_err_t esp_netif_up(esp_netif_t *esp_netif);
	esp_netif_up(netif);

	usb_netif = esp_netif_get_netif_impl(netif);
	usbd_cdc_net_register_rx_handler(received);

	return usb_netif;
}
