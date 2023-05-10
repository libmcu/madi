/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pnet/iface_usb.h"
#include "pusb/usbd.h"

#include <errno.h>
#include <string.h>
#include <semaphore.h>

#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#include "tusb.h"
#include "libmcu/board.h"

#define MTU		1514U

uint8_t tud_network_mac_address[6];

static struct netif usb_iface;
static sem_t rx_processing;

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

static int on_tx(const void *p, void *q)
{
	const struct pbuf *pbuf = (const struct pbuf *)p;
	return (int)pbuf_copy_partial(pbuf, q, pbuf->tot_len, 0);
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

static int received(const uint8_t *data, uint16_t datasize)
{
	if (sem_timedwait(&rx_processing, 0) != 0) {
		return 0;
	}

	if (datasize) {
		struct pbuf *p = pbuf_alloc(PBUF_RAW, datasize, PBUF_POOL);

		if (p) {
			memcpy(p->payload, data, datasize);
			if (usb_iface.input(p, &usb_iface) != ERR_OK) {
				pbuf_free(p);
			}
		}
	}

	sem_post(&rx_processing);

	return datasize;
}

struct netif *netif_create_usb(const struct netif_param *param)
{
	sem_init(&rx_processing, 0, 1);

	set_mac_address();
	usb_iface.hwaddr_len = sizeof(tud_network_mac_address);
	memcpy(usb_iface.hwaddr, tud_network_mac_address, usb_iface.hwaddr_len);
	usb_iface.hwaddr[5] ^= 0x01; /* to distinguish from the host's mac address. */

	struct netif *netif = netif_add(&usb_iface,
			(const ip4_addr_t *)&param->ip,
			(const ip4_addr_t *)&param->netmask,
			(const ip4_addr_t *)&param->gateway,
			NULL, init_cb, ethernet_input);

	usbd_cdc_net_register_rx_handler(received);

	return netif;
}

void netif_delete_usb(void *p)
{
	(void)p;
}
