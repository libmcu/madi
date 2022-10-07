/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/dhcpv4.h>

#include "libmcu/compiler.h"

#pragma push_macro("WIFI_MFP_REQUIRED")
#pragma push_macro("WIFI_MFP_OPTIONAL")
#define WIFI_MFP_REQUIRED	FPL_WIFI_MFP_REQUIRED
#define WIFI_MFP_OPTIONAL	FPL_WIFI_MFP_OPTIONAL
#define wifi_scan_result	fpl_wifi_scan_result
#include "net/wifi.h"
#undef wifi_scan_result
#pragma pop_macro("WIFI_MFP_OPTIONAL")
#pragma pop_macro("WIFI_MFP_REQUIRED")

#define WIFI_EVENTS (NET_EVENT_WIFI_SCAN_RESULT | \
			NET_EVENT_WIFI_SCAN_DONE | \
			NET_EVENT_WIFI_CONNECT_RESULT | \
			NET_EVENT_WIFI_DISCONNECT_RESULT)
#define DHCP_EVENTS (NET_EVENT_IPV4_DHCP_BOUND | NET_EVENT_IPV4_DHCP_STOP)

struct wifi_ctx {
	struct wifi base;

	struct net_if *iface;
	struct net_mgmt_event_callback wifi_cb;
	struct net_mgmt_event_callback dhcp_cb;
};

static struct wifi_ctx static_wifi_iface;

static void raise_event_with_data(struct wifi *iface,
				  enum wifi_event evt, const void *data)
{
	if (iface->callback) {
		(*iface->callback)(iface, evt, data);
	}
}

static enum wifi_security security_from_zephyr(uint8_t sec)
{
	switch (sec) {
	case WIFI_SECURITY_TYPE_PSK:
		return WIFI_SEC_TYPE_PSK;
	case WIFI_SECURITY_TYPE_PSK_SHA256:
		return WIFI_SEC_TYPE_PSK_SHA256;
	case WIFI_SECURITY_TYPE_SAE:
		return WIFI_SEC_TYPE_PSK_SAE;
	case WIFI_SECURITY_TYPE_NONE: /* fall through */
	default:
		return WIFI_SEC_TYPE_NONE;
	}
}

static void handle_scan_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_scan_result *entry =
		(const struct wifi_scan_result *)cb->info;
	struct fpl_wifi_scan_result res = {
		.ssid_len =  entry->ssid_length,
		.band = entry->band,
		.channel = entry->channel,
		.security = security_from_zephyr(entry->security),
		.mfp = entry->mfp,
		.rssi = entry->rssi,
		.mac_len = entry->mac_length,
	};

	memcpy(res.ssid, entry->ssid, entry->ssid_length);
	memcpy(res.mac, entry->mac, entry->mac_length);

	raise_event_with_data(&static_wifi_iface.base,
		       WIFI_EVT_SCAN_RESULT, &res);
}

static void handle_scan_done(void)
{
	raise_event_with_data(&static_wifi_iface.base, WIFI_EVT_SCAN_DONE, 0);
}

static void handle_connect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status = (const struct wifi_status *)cb->info;

	if (status->status == 0) {
		net_dhcpv4_start(static_wifi_iface.iface);
	} else {
		raise_event_with_data(&static_wifi_iface.base,
			WIFI_EVT_DISCONNECTED, 0);
	}
}

static void handle_disconnect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status = (const struct wifi_status *)cb->info;

	net_dhcpv4_stop(static_wifi_iface.iface);
	raise_event_with_data(&static_wifi_iface.base,
		WIFI_EVT_DISCONNECTED, 0);
}

static void on_wifi_events(struct net_mgmt_event_callback *cb,
				    uint32_t mgmt_event, struct net_if *iface)
{
	const struct wifi_status *rc = cb->info;

	switch (mgmt_event) {
	case NET_EVENT_WIFI_SCAN_RESULT:
		handle_scan_result(cb);
		break;
	case NET_EVENT_WIFI_SCAN_DONE:
		handle_scan_done();
		break;
	case NET_EVENT_WIFI_CONNECT_RESULT:
		handle_connect_result(cb);
		break;
	case NET_EVENT_WIFI_DISCONNECT_RESULT:
		handle_disconnect_result(cb);
		break;
	default:
		break;
	}
}

static void on_dhcp_events(struct net_mgmt_event_callback *cb,
				    uint32_t mgmt_event, struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_IPV4_DHCP_BOUND:
		memcpy(&static_wifi_iface.base.ip,
			&iface->config.ip.ipv4->unicast[0].address.in_addr, 4);
		raise_event_with_data(&static_wifi_iface.base,
			WIFI_EVT_CONNECTED, 0);
		break;
	default:
		break;
	}
}

static uint8_t band_from_fpl(enum wifi_frequency_band band)
{
	switch (band) {
	case WIFI_FREQ_5_GHZ:
		return WIFI_FREQ_BAND_5_GHZ;
	case WIFI_FREQ_6_GHZ:
		return WIFI_FREQ_BAND_6_GHZ;
	case WIFI_FREQ_2_4_GHZ: /* fall through */
	default:
		return WIFI_FREQ_BAND_2_4_GHZ;
	}
}

static uint8_t channel_from_fpl(uint8_t ch)
{
	if (ch == 0) {
		return WIFI_CHANNEL_ANY;
	}
	return ch;
}

static uint8_t security_from_fpl(enum wifi_security sec)
{
	switch (sec) {
	case WIFI_SEC_TYPE_PSK:
		return WIFI_SECURITY_TYPE_PSK;
	case WIFI_SEC_TYPE_PSK_SHA256:
		return WIFI_SECURITY_TYPE_PSK_SHA256;
	case WIFI_SEC_TYPE_PSK_SAE:
		return WIFI_SECURITY_TYPE_SAE;
	case WIFI_SEC_TYPE_WEP: /* Not supported. fall through */
	case WIFI_SEC_TYPE_UNKNOWN: /* fall through */
	case WIFI_SEC_TYPE_NONE: /* fall through */
	default:
		return WIFI_SECURITY_TYPE_NONE;
	}
}

static uint8_t mfp_from_fpl(enum wifi_mfp mfp)
{
	switch (mfp) {
	case FPL_WIFI_MFP_OPTIONAL:
		return WIFI_MFP_OPTIONAL;
	case FPL_WIFI_MFP_REQUIRED:
		return WIFI_MFP_REQUIRED;
	case WIFI_MFP_DISABLED: /* fall through */
	default:
		return WIFI_MFP_DISABLE;
	}
}

static int do_connect(struct wifi *iface, const struct wifi_conn_param *param)
{
	struct net_if *net_if = ((struct wifi_ctx *)iface)->iface;
	struct wifi_connect_req_params conf = {
		.ssid = param->ssid,
		.ssid_length = param->ssid_len,
		.psk = param->psk,
		.psk_length = param->psk_len,
		.band = band_from_fpl(param->band),
		.channel = channel_from_fpl(param->channel),
		.security = security_from_fpl(param->security),
		.mfp = mfp_from_fpl(param->mfp),
		.timeout = param->timeout_ms,
	};

	if (net_mgmt(NET_REQUEST_WIFI_CONNECT, net_if, &conf, sizeof(conf))) {
		return -ENOEXEC;
	}

	memcpy(iface->status.ssid, param->ssid, param->ssid_len);
	iface->status.ssid_len = param->ssid_len;
	iface->status.security = param->security;

	return 0;
}

static int do_disconnect(struct wifi *iface)
{
	struct net_if *net_if = ((struct wifi_ctx *)iface)->iface;

	return net_mgmt(NET_REQUEST_WIFI_DISCONNECT, net_if, NULL, 0);
}

static int do_scan(struct wifi *iface)
{
	struct net_if *net_if = ((struct wifi_ctx *)iface)->iface;

	if (net_mgmt(NET_REQUEST_WIFI_SCAN, net_if, NULL, 0)) {
		return -ENOEXEC;
	}

	return 0;
}

static int do_start(struct wifi *iface)
{
	raise_event_with_data(iface, WIFI_EVT_STARTED, 0);
	return 0;
}

static int do_stop(struct wifi *iface)
{
	raise_event_with_data(iface, WIFI_EVT_STOPPED, 0);
	return 0;
}

static int do_get_status(struct wifi *iface, struct wifi_iface_info *info)
{
	memcpy(info, &iface->status, sizeof(iface->status));
	return 0;
}

static int do_register_event_callback(struct wifi *iface,
		const wifi_event_callback_t cb)
{
	iface->callback = cb;
	return 0;
}

struct wifi *wifi_create_default(void)
{
	static_wifi_iface.iface = net_if_get_default();

	if (static_wifi_iface.iface == NULL) {
		return NULL;
	}

	net_mgmt_init_event_callback(&static_wifi_iface.wifi_cb,
				     on_wifi_events, WIFI_EVENTS);
	net_mgmt_add_event_callback(&static_wifi_iface.wifi_cb);
	net_mgmt_init_event_callback(&static_wifi_iface.dhcp_cb,
				     on_dhcp_events, DHCP_EVENTS);
	net_mgmt_add_event_callback(&static_wifi_iface.dhcp_cb);

	memcpy(&static_wifi_iface.base.status.mac,
			net_if_get_link_addr(static_wifi_iface.iface)->addr,
			WIFI_MAC_ADDR_LEN);

	static_wifi_iface.base.api.connect = do_connect;
	static_wifi_iface.base.api.disconnect = do_disconnect;
	static_wifi_iface.base.api.scan = do_scan;
	static_wifi_iface.base.api.start = do_start;
	static_wifi_iface.base.api.stop = do_stop;
	static_wifi_iface.base.api.get_status = do_get_status;
	static_wifi_iface.base.api.register_event_callback =
			do_register_event_callback;

	return &static_wifi_iface.base;
}

void wifi_destroy_default(struct wifi *iface)
{
	unused(iface);
}
