/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "pnet/net.h"
#include "pnet/iface_usb.h"
#include "pusb/usbd.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/stats.h"
#include "lwip/apps/mdns.h"
#include "dhserver.h"

#include "libmcu/board.h"
#include "libmcu/logging.h"

#include "httpsrv.h"

/* TODO: A random third-part IP address would help avoid conflicts on the host
 * side. */
static dhcp_entry_t entries[] = {
	{ {0}, {PP_HTONL(LWIP_MAKEU32(192, 168, 13, 2))}, 24 * 60 * 60 },
};

static const dhcp_config_t dhcp_config = {
	.router = {PP_HTONL(LWIP_MAKEU32(0, 0, 0, 0))},
	.port = 67,
	.dns = {PP_HTONL(LWIP_MAKEU32(192, 168, 13, 1))},
	.domain = "usb",
	.num_entry = sizeof(entries) / sizeof(entries[0]),
	.entries = entries,
};

/* NOTE: For ESP32S3, LWIP integrated into ESP-IDF used. */
#if !defined(esp32s3)
static void on_mdns_report(struct netif* netif, u8_t result, s8_t slot)

{
	debug("mdns status[netif %d]: %d, %d", netif->num, result, slot);
}

static void on_mdns_service_set(struct mdns_service *service,
		void *txt_userdata)
{
	(void)txt_userdata;

	if (mdns_resp_add_service_txtitem(service, "path=/", 6) != ERR_OK) {
		error("mdns add service txt failed");
	}
}

static void check_lwip_timeouts(void *ctx)
{
	(void)ctx;
	sys_check_timeouts();
}

uint32_t sys_now(void)
{
	return board_get_time_since_boot_ms();
}
#endif /* End of !defined(esp32s3) */

int net_init(void)
{
#if !defined(esp32s3)
	lwip_init();
	usbd_register_periodic_callback(check_lwip_timeouts, 0);
#endif

	struct netif_param param = {
		.ip = PP_HTONL(LWIP_MAKEU32(192,168,13,1)),
		.netmask = PP_HTONL(LWIP_MAKEU32(255,255,255,0)),
		.gateway = PP_HTONL(LWIP_MAKEU32(0,0,0,0)),
	};
	struct netif *iface = netif_create_usb(&param);
#if LWIP_IPV6
	netif_create_ip6_linklocal_address(iface, 1);
#endif
	netif_set_default(iface);

	while (!netif_is_up(iface));
	while (dhserv_init(&dhcp_config) != ERR_OK);

#if !defined(esp32s3)
	mdns_resp_register_name_result_cb(on_mdns_report);
	mdns_resp_init();
	mdns_resp_add_netif(iface, "madi");
	mdns_resp_add_service(iface, "madi", "_http", DNSSD_PROTO_TCP, 80,
			on_mdns_service_set, NULL);
	mdns_resp_announce(iface);

	httpsrv_init();
#endif

	return 0;
}
