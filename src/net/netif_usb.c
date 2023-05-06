/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "netif_usb.h"
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#define MTU		1514U

static struct netif usb_iface;
static sem_t rx_processing;

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
	(void)netif;
	return usbd_cdc_net_send(p, p->tot_len) == ERR_OK? ERR_OK : ERR_USE;
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

int usbd_cdc_net_received(const void *data, uint16_t datasize)
{
	if (sem_timedwait(&rx_processing, 0) != 0) {
		return -EALREADY;
	}

	if (datasize) {
		struct pbuf *p = pbuf_alloc(PBUF_RAW, datasize, PBUF_POOL);

		if (p) {
			memcpy(p->payload, data, datasize);
			usb_iface.input(p, &usb_iface);
			pbuf_free(p);
			usbd_cdc_net_received_post();
		}
	}

	sem_post(&rx_processing);

	return 0;
}

int usbd_cdc_net_send_post(const void *p, void *q)
{
	const struct pbuf *pbuf = (const struct pbuf *)p;
	return (int)pbuf_copy_partial(pbuf, q, pbuf->tot_len, 0);
}

void *netif_usb_create(const struct net_iface_param *param)
{
	sem_init(&rx_processing, 0, 1);

	usb_iface.hwaddr_len = 6;
	usbd_cdc_net_get_mac(usb_iface.hwaddr);

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
