/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/cli.h"

#include <stdio.h>
#include <string.h>
#include <semaphore.h>

#include "pwifi/wifi.h"

#include "libmcu/hexdump.h"
#include "libmcu/compiler.h"

static const struct cli_io *io;
static unsigned int scan_index;
static sem_t events;

static void println(const char *str)
{
	io->write(str, strlen(str));
	io->write("\n", 1);
}

static const char *stringify_band(enum wifi_frequency_band band)
{
	switch (band) {
	case WIFI_FREQ_2_4_GHZ:
		return "2.4GHz";
	case WIFI_FREQ_5_GHZ:
		return "5GHz";
	case WIFI_FREQ_6_GHZ:
		return "6GHz";
	default:
		return "Unknown";
	}
}

static const char *stringify_security(enum wifi_security sec)
{
	switch (sec) {
	case WIFI_SEC_TYPE_NONE:
		return "Open";
	case WIFI_SEC_TYPE_WEP:
		return "WEP";
	case WIFI_SEC_TYPE_PSK:
		return "PSK";
	case WIFI_SEC_TYPE_PSK_SHA256:
		return "PSK_SHA256";
	case WIFI_SEC_TYPE_PSK_SAE:
		return "PSK_SAE";
	default:
		return "Unknown";
	}
}

static void print_scan_result(const struct wifi_scan_result *entry)
{
	if (entry == NULL) {
		return;
	}

	char buf[128] = { 0, };
	char mac[20] = { 0, };

	if (scan_index == 0) {
		snprintf(buf, sizeof(buf),
			"\n%-4s | %-32s | %-13s | %-4s | %-10s | %s",
			"No.", "SSID", "Chan (Band)", "RSSI", "Security", "BSSID");
		println(buf);
	}

	scan_index++;

	hexdump(mac, sizeof(mac), entry->mac, entry->mac_len);
	snprintf(buf, sizeof(buf),
			"%-4d | %.*s%s | %-4u (%-6s) | %-4d | %-10s | %s",
			scan_index, entry->ssid_len, entry->ssid,
			&"                                "[entry->ssid_len],
			entry->channel, stringify_band(entry->band),
			entry->rssi, stringify_security(entry->security), mac);
	println(buf);
}

static void on_wifi_events(const struct wifi *iface,
			   enum wifi_event evt, const void *data)
{
	char buf[16];
	unused(iface);

	switch (evt) {
	case WIFI_EVT_SCAN_RESULT:
		print_scan_result((const struct wifi_scan_result *)data);
		break;
	case WIFI_EVT_SCAN_DONE: /* fall through */
	case WIFI_EVT_STARTED: /* fall through */
	case WIFI_EVT_DISCONNECTED: /* fall through */
	case WIFI_EVT_CONNECTED: /* fall through */
	default:
		snprintf(buf, sizeof(buf), "WIFI EVT: %x", evt);
		println(buf);
		sem_post(&events);
		break;
	}
}

static void print_wifi_info(struct wifi *iface)
{
	struct wifi_iface_info info;

	if (iface == NULL) {
		return;
	}

	wifi_get_status(iface, &info);
	char buf[WIFI_MAC_ADDR_LEN*2+4/*dots*/+2/*crlf*/];
	hexdump(buf, sizeof(buf), info.mac, sizeof(info.mac));
	println(buf);
	snprintf(buf, sizeof(buf), "%d.%d.%d.%d",
			iface->ip.v4[0], iface->ip.v4[1],
			iface->ip.v4[2], iface->ip.v4[3]);
	println(buf);
	snprintf(buf, sizeof(buf), "rssi %d", info.rssi);
	println(buf);
}

static struct wifi *handle_single_param(const char *argv[], struct wifi *iface)
{
	if (strcmp(argv[1], "help") == 0) {
		println("subcommands:\n\n\tinit\n\tenable\n\tdisable\n\tscan" \
			"\n\tconnect\t: <ssid> <pass>\n\tdisconnect");
		return iface;
	} else if (strcmp(argv[1], "init") == 0) {
		return wifi_create_default();
	} else if (iface == NULL) {
		return NULL;
	}

	bool wait_on_events = true;

	if (strcmp(argv[1], "enable") == 0) {
		wifi_register_event_callback(iface, on_wifi_events);
		wifi_enable(iface);
	} else if (strcmp(argv[1], "disable") == 0) {
		wifi_disable(iface);
	} else if (strcmp(argv[1], "scan") == 0) {
		if (wifi_scan(iface) == 0) {
			scan_index = 0;
		}
	} else if (strcmp(argv[1], "disconnect") == 0) {
		wifi_disconnect(iface);
	} else {
		wait_on_events = false;
	}

	if (wait_on_events) {
		sem_wait(&events);
	}

	return iface;
}

static void handle_multi_params(int argc, const char *argv[], struct wifi *iface)
{
	if (strcmp(argv[1], "connect") == 0 && argc >= 3) {
		struct wifi_conn_param param = {
			.ssid = (const uint8_t *)argv[2],
			.ssid_len = (uint8_t)strlen(argv[2]),
			.security = WIFI_SEC_TYPE_NONE,
		};

		if (argc >= 4) {
			param.psk = (const uint8_t *)argv[3];
			param.psk_len = (uint8_t)strlen(argv[3]);
			param.security = WIFI_SEC_TYPE_PSK;
		}

		if (argc == 5 && strcmp(argv[4], "wep") == 0) {
			param.security = WIFI_SEC_TYPE_WEP;
		}

		wifi_connect(iface, &param);
		sem_wait(&events);
	}
}

DEFINE_CLI_CMD(wifi, "WiFi functions") {
	static struct wifi *iface;
	struct cli const *cli = (struct cli const *)env;

	io = cli->io;

	if (sem_init(&events, 0, 0) != 0) {
		goto out;
	}

	if (argc == 1 && iface) {
		print_wifi_info(iface);
	} else if (argc == 2) {
		iface = handle_single_param(argv, iface);
	} else if (argc > 1 && iface) {
		handle_multi_params(argc, argv, iface);
	}

out:
	sem_destroy(&events);

	return CLI_CMD_SUCCESS;
}
