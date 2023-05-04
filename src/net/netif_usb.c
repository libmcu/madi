/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "netif_usb.h"
#include <errno.h>
#include <string.h>
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#define MTU		1514U

static struct netif usb_iface;
static struct pbuf *received_frame;

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
	(void)netif;
	return netif_usb_output(p, p->tot_len) == ERR_OK? ERR_OK : ERR_USE;
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

int netif_usb_clean_input(bool xfer)
{
	if (received_frame) {
		pbuf_free(received_frame);
		received_frame = NULL;
		if (xfer) {
			netif_usb_input_done();
		}
	}

	return 0;
}

int netif_usb_input(const void *data, uint16_t datasize)
{
	if (received_frame) {
		return -EALREADY;
	}

	if (datasize) {
		struct pbuf *p = pbuf_alloc(PBUF_RAW, datasize, PBUF_POOL);

		if (p) {
			memcpy(p->payload, data, datasize);
			received_frame = p;
		}
	}

	return 0;
}

int netif_usb_clean_output(const void *p, void *q)
{
	const struct pbuf *pbuf = (const struct pbuf *)p;
	return (int)pbuf_copy_partial(pbuf, q, pbuf->tot_len, 0);
}

int netif_usb_step(void)
{
	if (received_frame) {
		usb_iface.input(received_frame, &usb_iface);
		netif_usb_clean_input(true);
	}

	return 0;
}

void *netif_usb_create(const struct net_iface_param *param)
{
	usb_iface.hwaddr_len = 6;
	netif_usb_get_mac(&usb_iface, usb_iface.hwaddr);

	return (void *)netif_add(&usb_iface,
			(const ip4_addr_t *)&param->ip,
			(const ip4_addr_t *)&param->netmask,
			(const ip4_addr_t *)&param->gateway,
			NULL, init_cb, ethernet_input);
}

void netif_usb_delete(void *p)
{
	(void)p;
}
