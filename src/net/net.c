/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "net.h"
#include "netif_usb.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/stats.h"
#include "dhserver.h"
#include "httpd.h"

#include "pusb/usbd.h"

#include "libmcu/board.h"

static dhcp_entry_t entries[] = {
	{ {0}, {PP_HTONL(LWIP_MAKEU32(192, 168, 7, 2))}, 24 * 60 * 60 },
	{ {0}, {PP_HTONL(LWIP_MAKEU32(192, 168, 7, 3))}, 24 * 60 * 60 },
	{ {0}, {PP_HTONL(LWIP_MAKEU32(192, 168, 7, 4))}, 24 * 60 * 60 },
};

static const dhcp_config_t dhcp_config = {
	.router = {PP_HTONL(LWIP_MAKEU32(0, 0, 0, 0))},
	.port = 67,
	.dns = {PP_HTONL(LWIP_MAKEU32(192, 168, 7, 1))},
	.domain = "usb",
	.num_entry = sizeof(entries) / sizeof(entries[0]),
	.entries = entries,
};

sys_prot_t sys_arch_protect(void)
{
	return 0;
}
void sys_arch_unprotect(sys_prot_t pval)
{
	(void)pval;
}

uint32_t sys_now(void)
{
	return board_get_time_since_boot_ms();
}

int net_step(void)
{
	usbd_step();
	sys_check_timeouts();

	return 0;
}

int net_init(void)
{
	usbd_init();

	struct net_iface_param param = {
		.ip = PP_HTONL(LWIP_MAKEU32(192,168,7,1)),
		.netmask = PP_HTONL(LWIP_MAKEU32(255,255,255,0)),
		.gateway = PP_HTONL(LWIP_MAKEU32(0,0,0,0)),
	};

	lwip_init();
	struct netif *iface = (struct netif *)netif_usb_create(&param);
#if LWIP_IPV6
	netif_create_ip6_linklocal_address(iface, 1);
#endif
	netif_set_default(iface);

	while (!netif_is_up(iface));
	while (dhserv_init(&dhcp_config) != ERR_OK);
httpd_init();

	return 0;
//stats_display();
}
