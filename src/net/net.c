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

static void run_dhcp_server(void)
{
	/* TODO: A random third-part IP address would help avoid conflicts on
	 * the host side. */
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

	while (dhserv_init(&dhcp_config) != ERR_OK);
}

uint32_t sys_now(void)
{
	return board_get_time_since_boot_ms();
}

#define AWS_CA				\
"-----BEGIN CERTIFICATE-----\n" \
"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
"-----END CERTIFICATE-----\n"

#include "psa/crypto.h"
#include "lwip/altcp_tls.h"
#include "lwip/ip_addr.h"

static err_t on_received_test(void *arg, struct altcp_pcb *conn,
		struct pbuf *p, err_t err)
{
    if (p == NULL) {
        altcp_close(conn);
        return ERR_ABRT;
    } else {
        debug("Number of pbufs %d", pbuf_clen(p));
        debug("Contents of pbuf %s", (char *)p->payload);
    }

    return 0;
}

static err_t on_sent_test(void *arg, struct altcp_pcb *conn, u16_t len)
{
	debug("sent!");
	return 0;
}

static void on_error_test(void *arg, err_t err)
{
	error("err %d", err);
}

static err_t on_connected_test(void *arg, struct altcp_pcb *conn, err_t err)
{
	debug("connected!");
    char *string = "HEAD /process.php?data1=12&data2=5 HTTP/1.0\r\nHost: mywebsite.com\r\n\r\n ";
    uint32_t len = strlen(string);
	if (altcp_write(conn, string, strlen(string), TCP_WRITE_FLAG_COPY)) {
		error("tx: can not buffer");
	}
	if (altcp_output(conn)) {
		error("tx: can not output");
	}
    return 0;
}

#if 0
static void my_debug(void *ctx, int level,
                     const char *file, int line,
                     const char *str)
{
    ((void) level);

    fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
    fflush((FILE *) ctx);
}
#endif

void test_tls(void)
{
	mbedtls_debug_set_threshold(5);

	psa_crypto_init();

	struct altcp_tls_config *cfg =
		altcp_tls_create_config_client(AWS_CA, sizeof(AWS_CA));
	altcp_allocator_t allocator = {
		.alloc = altcp_tls_alloc,
		.arg = cfg,
	};
	//mbedtls_ssl_conf_dbg(&cfg->conf, my_debug, stdout);

	struct altcp_pcb *pcb = altcp_new(&allocator);

    ip_addr_t ip;
    IP4_ADDR(&ip, 192,168,13,2);

    uint32_t data = 0xdeadbeef;
    altcp_arg(pcb, &data);

    altcp_err(pcb, on_error_test);
    altcp_recv(pcb, on_received_test);
    altcp_sent(pcb, on_sent_test);

	//mbedtls_ssl_set_hostname(altcp_tls_context(pcb), "libmcu.org");
    err_t err = altcp_connect(pcb, &ip, 60000, on_connected_test);
}

__attribute__((visibility("default")))
int net_init(void)
{
	lwip_init();
	usbd_register_periodic_callback(check_lwip_timeouts, 0);

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

	run_dhcp_server();

	mdns_resp_register_name_result_cb(on_mdns_report);
	mdns_resp_init();
	mdns_resp_add_netif(iface, "madi");
	mdns_resp_add_service(iface, "madi", "_http", DNSSD_PROTO_TCP, 80,
			on_mdns_service_set, NULL);
	mdns_resp_announce(iface);

	/* TODO: implement portable http server */
	httpsrv_init();

	return 0;
}
