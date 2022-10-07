/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "net/wifi.h"
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"

enum esp32_state {
	ESP32_STATE_UNKNOWN,
	ESP32_STATE_STARTING,
	ESP32_STATE_STA_STOPPED,
	ESP32_STATE_STA_STARTED,
	ESP32_STATE_STA_CONNECTING,
	ESP32_STATE_STA_CONNECTED,
	ESP32_STATE_AP_STOPPED,
	ESP32_STATE_AP_CONNECTED,
	ESP32_STATE_AP_DISCONNECTED,
};

struct esp_wifi {
	struct wifi base;

	esp_event_handler_instance_t wifi_events;
	esp_event_handler_instance_t ip_acquisition_events;

	enum esp32_state state;
};

static struct esp_wifi static_esp_iface;

static void raise_event_with_data(struct wifi *iface,
				  enum wifi_event evt, const void *data)
{
	if (iface->callback) {
		(*iface->callback)(iface, evt, data);
	}
}

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

		raise_event_with_data(&static_esp_iface.base,
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
	raise_event_with_data(&static_esp_iface.base, WIFI_EVT_SCAN_DONE, 0);
}

static void handle_scan_done(void)
{
	handle_scan_result();
}

static void handle_connected_event(struct esp_wifi *ctx,
		ip_event_got_ip_t *ip)
{
	ctx->state = ESP32_STATE_STA_CONNECTED;
	memcpy(&ctx->base.ip, &ip->ip_info.ip, sizeof(ctx->base.ip));
	raise_event_with_data(&ctx->base, WIFI_EVT_CONNECTED, 0);
}


/* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/event-handling.html#event-ids-and-corresponding-data-structures */
static void on_wifi_events(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data)
{
	switch (event_id) {
	case WIFI_EVENT_STA_START:
		static_esp_iface.state = ESP32_STATE_STA_STARTED;
		raise_event_with_data(&static_esp_iface.base,
				WIFI_EVT_STARTED, 0);
		break;
	case WIFI_EVENT_STA_STOP:
		static_esp_iface.state = ESP32_STATE_STA_STOPPED;
		raise_event_with_data(&static_esp_iface.base,
				WIFI_EVT_STOPPED, 0);
		break;
	case WIFI_EVENT_STA_CONNECTED:
		/* The connection event will be raised when IP acquired as DHCP
		 * is getting started automatically by netif which is
		 * registered earlier at the initailization. */
		break;
	case WIFI_EVENT_STA_DISCONNECTED:
		static_esp_iface.state = ESP32_STATE_STA_STARTED;
		memset(&static_esp_iface.base.ip, 0,
				sizeof(static_esp_iface.base.ip));
		raise_event_with_data(&static_esp_iface.base,
				WIFI_EVT_DISCONNECTED, 0);
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
		handle_connected_event(&static_esp_iface, event_data);
		break;
	case IP_EVENT_STA_LOST_IP:
	default:
		break;
	}
}

static bool initialize_wifi_event(struct esp_wifi *ctx)
{
	esp_err_t res = esp_event_handler_instance_register(WIFI_EVENT,
			ESP_EVENT_ANY_ID,
			&on_wifi_events,
			NULL,
			&ctx->wifi_events);
	res |= esp_event_handler_instance_register(IP_EVENT,
			ESP_EVENT_ANY_ID,
			&on_ip_events,
			NULL,
			&ctx->ip_acquisition_events);

	return !res;
}

static bool initialize_wifi_iface(struct esp_wifi *ctx)
{
	wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
	esp_err_t res = esp_netif_init();

	if (esp_netif_create_default_wifi_sta() == NULL) {
		return false;
	}

	res |= esp_wifi_init(&config);
	res |= esp_wifi_set_mode(WIFI_MODE_STA);

	return !res;
}

static int initialize_wifi(struct esp_wifi *ctx)
{
	if (!initialize_wifi_event(ctx)) {
		return -EBUSY;
	}
	if (!initialize_wifi_iface(ctx)) {
		return -EAGAIN;
	}

	return 0;
}

static int deinitialize_wifi(struct esp_wifi *ctx)
{
	return esp_wifi_deinit() == ESP_OK ? 0 : -EBUSY;
}

static int do_connect(struct wifi *iface, const struct wifi_conn_param *param)
{
	struct esp_wifi *ctx = (struct esp_wifi *)iface;

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

	if (ctx->state == ESP32_STATE_STA_CONNECTING ||
			ctx->state == ESP32_STATE_STA_CONNECTED) {
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

	ctx->state = ESP32_STATE_STA_CONNECTING;

	if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK ||
			esp_wifi_connect() != ESP_OK) {
		return -ENOTCONN;
	}

	memcpy(iface->status.ssid, param->ssid, param->ssid_len);
	iface->status.ssid_len = param->ssid_len;
	iface->status.security = param->security;

	return 0;
}

static int do_disconnect(struct wifi *iface)
{
	struct esp_wifi *ctx = (struct esp_wifi *)iface;

	if (ctx->state != ESP32_STATE_STA_CONNECTED &&
			ctx->state != ESP32_STATE_STA_CONNECTING) {
		return -ENOTCONN;
	}

	if (esp_wifi_disconnect() != ESP_OK) {
		return -EAGAIN;
	}

	return 0;
}

static int do_scan(struct wifi *iface)
{
	struct esp_wifi *ctx = (struct esp_wifi *)iface;

	if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK ||
			esp_wifi_scan_start(&(wifi_scan_config_t) {
						.show_hidden = true,
					}, false) != ESP_OK) {
		return -EAGAIN;
	}

	return 0;
}

static int do_start(struct wifi *iface)
{
	return esp_wifi_start() == ESP_OK ? 0 : -EAGAIN;
}

static int do_stop(struct wifi *iface)
{
	return esp_wifi_stop() == ESP_OK ? 0 : -EBUSY;
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

struct wifi *esp_wifi_create(void)
{
	if (static_esp_iface.state != ESP32_STATE_UNKNOWN) {
		return NULL;
	}

	static_esp_iface.state = ESP32_STATE_STARTING;

	static_esp_iface.base.api.connect = do_connect;
	static_esp_iface.base.api.disconnect = do_disconnect;
	static_esp_iface.base.api.scan = do_scan;
	static_esp_iface.base.api.start = do_start;
	static_esp_iface.base.api.stop = do_stop;
	static_esp_iface.base.api.get_status = do_get_status;
	static_esp_iface.base.api.register_event_callback =
			do_register_event_callback;

	if (initialize_wifi(&static_esp_iface)) {
		deinitialize_wifi(&static_esp_iface);
		static_esp_iface.state = ESP32_STATE_UNKNOWN;
		return NULL;
	}

	esp_read_mac(static_esp_iface.base.status.mac, ESP_MAC_WIFI_STA);
	static_esp_iface.base.status.mac_len = WIFI_MAC_ADDR_LEN;

	return (struct wifi *)&static_esp_iface;
}

void esp_wifi_destroy(struct wifi *iface)
{
	struct esp_wifi *ctx = (struct esp_wifi *)iface;

	ctx->state = ESP32_STATE_UNKNOWN;
}
