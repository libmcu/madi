/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "common/wifi.h"
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"

struct esp_wifi_iface {
	struct wifi_iface base;

	esp_event_handler_instance_t wifi_events;
	esp_event_handler_instance_t ip_acquisition_events;
};

static struct esp_wifi_iface esp_iface;

static void handle_scan_result_core(uint16_t n, const wifi_ap_record_t *scanned)
{
	struct wifi_scan_result res = { 0, };

	for (int i = 0; i < n; i++) {
		res.ssid_len = strnlen(scanned[i].ssid, WIFI_SSID_MAX_LEN);
		strncpy(res.ssid, scanned[i].ssid, res.ssid_len);
		res.rssi = scanned[i].rssi;
		res.channel = scanned[i].primary;
		res.security = WIFI_SEC_TYPE_NONE;
		if (scanned[i].authmode == WIFI_AUTH_WEP) {
			res.security = WIFI_SEC_TYPE_WEP;
		} else if (scanned[i].authmode > WIFI_AUTH_OPEN) {
			res.security = WIFI_SEC_TYPE_PSK;
		}
		res.mac_len = WIFI_MAC_ADDR_LEN;
		memcpy(res.mac, scanned[i].bssid, res.mac_len);

		wifi_raise_event_with_data((wifi_iface_t)&esp_iface,
				WIFI_EVT_SCAN_RESULT, &res);
	}
}

static void handle_scan_result(void)
{
	uint16_t n = 0;
	wifi_ap_record_t *scanned;

	esp_wifi_scan_get_ap_num(&n);
	if (n == 0) {
		goto out;
	}
	if ((scanned = (wifi_ap_record_t *)calloc(n, sizeof(wifi_ap_record_t)))
			== NULL) {
		goto out;
	}

	if (esp_wifi_scan_get_ap_records(&n, scanned) == ESP_OK) {
		handle_scan_result_core(n, scanned);
	}

	free(scanned);
out:
	wifi_raise_event((wifi_iface_t)&esp_iface, WIFI_EVT_SCAN_DONE);
}

static void handle_scan_done(void)
{
	wifi_set_state((wifi_iface_t)&esp_iface, esp_iface.base.state_prev);
	handle_scan_result();
}

static void on_wifi_events(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data)
{
	switch (event_id) {
	case WIFI_EVENT_STA_START:
		wifi_set_state((wifi_iface_t)&esp_iface, WIFI_STATE_INACTIVE);
		break;
	case WIFI_EVENT_STA_CONNECTED:
		/* The connection event will be raised when IP acquired as DHCP
		 * is getting started automatically by netif which is
		 * registered earlier at the initailization. */
		break;
	case WIFI_EVENT_STA_DISCONNECTED:
		wifi_set_state((wifi_iface_t)&esp_iface, WIFI_STATE_DISCONNECTED);
		memset(&esp_iface.base.ip, 0, sizeof(esp_iface.base.ip));
		break;
	case WIFI_EVENT_SCAN_DONE:
		handle_scan_done();
		break;
	case WIFI_EVENT_AP_START:
	case WIFI_EVENT_AP_STOP:
	case WIFI_EVENT_AP_STACONNECTED:
	case WIFI_EVENT_AP_STADISCONNECTED:
	default:
		break;
	}
}

static void on_ip_events(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data)
{
	switch (event_id) {
	case IP_EVENT_STA_GOT_IP:
		wifi_set_state((wifi_iface_t)&esp_iface, WIFI_STATE_ASSOCIATED);
		memcpy(&esp_iface.base.ip,
				&((ip_event_got_ip_t *)event_data)->ip_info.ip,
				sizeof(esp_iface.base.ip));
		break;
	default:
		break;
	}
}

static bool initialize_wifi_event(void)
{
	esp_err_t res = esp_event_handler_instance_register(WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			&on_wifi_events,
			NULL,
			&esp_iface.wifi_events);
	res |= esp_event_handler_instance_register(IP_EVENT,
			ESP_EVENT_ANY_ID,
			&on_ip_events,
			NULL,
			&esp_iface.ip_acquisition_events);

	return !res;
}

static bool initialize_wifi_iface(void)
{
	wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
	esp_err_t res = esp_netif_init();

	if (esp_netif_create_default_wifi_sta() == NULL) {
		return false;
	}

	res |= esp_wifi_init(&config);
	res |= esp_wifi_set_mode(WIFI_MODE_STA);
	res |= esp_wifi_start();

	return !res;
}

wifi_iface_t wifi_create(void)
{
	wifi_set_mode((wifi_iface_t)&esp_iface, WIFI_MODE_INFRA);
	wifi_set_state((wifi_iface_t)&esp_iface, WIFI_STATE_DISABLED);

	if (!initialize_wifi_event()) {
		return NULL;
	}
	if (!initialize_wifi_iface()) {
		return NULL;
	}

	esp_efuse_mac_get_default(esp_iface.base.mac);

	return (wifi_iface_t)&esp_iface;
}

int wifi_connect(wifi_iface_t iface, const struct wifi_conf *param)
{
	wifi_config_t conf = {
		.sta = {
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,
			.scan_method = WIFI_ALL_CHANNEL_SCAN,
			.sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
			.pmf_cfg = {
				.capable = true,
				.required = false
			},
		},
	};

	if (iface->mode != WIFI_MODE_INFRA) {
		return -ENOTSUP;
	}
	if (iface->state != WIFI_STATE_INACTIVE &&
			iface->state != WIFI_STATE_DISCONNECTED) {
		return -EISCONN;
	}
	if (sizeof(conf.sta.ssid) < param->ssid_len ||
			sizeof(conf.sta.password) < param->psk_len) {
		return -ERANGE;
	}

	memcpy(conf.sta.ssid, param->ssid, param->ssid_len);
	memcpy(conf.sta.password, param->psk, param->psk_len);

	if (param->security == WIFI_SEC_TYPE_NONE) {
		conf.sta.threshold.authmode = WIFI_AUTH_OPEN;
	} else if (param->security == WIFI_SEC_TYPE_WEP) {
		conf.sta.threshold.authmode = WIFI_AUTH_WEP;
	}

	if (esp_wifi_set_config(WIFI_IF_STA, &conf) != ESP_OK) {
		return -EINVAL;
	}

	wifi_set_state(iface, WIFI_STATE_ASSOCIATING);

	if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK ||
			esp_wifi_connect() != ESP_OK) {
		return -ENOTCONN;
	}

	return 0;
}

int wifi_disconnect(wifi_iface_t iface)
{
	if (iface->mode != WIFI_MODE_INFRA) {
		return -ENOTSUP;
	}
	if (iface->state != WIFI_STATE_ASSOCIATED &&
			iface->state != WIFI_STATE_ASSOCIATING) {
		return -ENOTCONN;
	}

	if (esp_wifi_disconnect() != ESP_OK) {
		return -EAGAIN;
	}

	wifi_set_state(iface, WIFI_STATE_DISCONNECTING);

	return 0;
}

int wifi_scan(wifi_iface_t iface)
{
	if (iface->state == WIFI_STATE_SCANNING) {
		return -EINPROGRESS;
	}

	if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK ||
			esp_wifi_scan_start(&(wifi_scan_config_t) {
						.show_hidden = true,
					}, false) != ESP_OK) {
		return -EAGAIN;
	}

	wifi_set_state(iface, WIFI_STATE_SCANNING);

	return 0;
}

int wifi_get_ap_info(wifi_iface_t iface, struct wifi_ap_info *info)
{
	wifi_ap_record_t esp_info;

	if (esp_wifi_sta_get_ap_info(&esp_info) != ESP_OK) {
		return -EAGAIN;
	}

	iface->rssi = esp_info.rssi;

	if (info == NULL) {
		goto out;
	}

	info->channel = esp_info.primary;
	info->rssi = esp_info.rssi;
	info->ssid_len = strnlen(esp_info.ssid, WIFI_SSID_MAX_LEN);
	memcpy(info->ssid, esp_info.ssid, info->ssid_len);
	info->mac_len = WIFI_MAC_ADDR_LEN;
	memcpy(info->mac, esp_info.bssid, info->mac_len);

	enum wifi_security sec = WIFI_SEC_TYPE_UNKNOWN;

	switch (esp_info.authmode) {
	case WIFI_AUTH_OPEN:
		sec = WIFI_SEC_TYPE_NONE;
		break;
	case WIFI_AUTH_WEP:
		sec = WIFI_SEC_TYPE_WEP;
		break;
	case WIFI_AUTH_WPA_PSK:
	case WIFI_AUTH_WPA2_PSK:
	case WIFI_AUTH_WPA_WPA2_PSK:
		sec = WIFI_SEC_TYPE_PSK;
		break;
	case WIFI_AUTH_WPA3_PSK:
	case WIFI_AUTH_WPA2_WPA3_PSK:
	case WIFI_AUTH_WAPI_PSK:
	case WIFI_AUTH_WPA2_ENTERPRISE:
		/* fall through */
	default:
		break;
	}

	info->security = sec;

out:
	return 0;
}
