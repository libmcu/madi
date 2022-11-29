/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_WIFI_H
#define FPL_WIFI_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

#define WIFI_SSID_MAX_LEN		32U
#define WIFI_MAC_ADDR_LEN		6U

enum wifi_event {
	WIFI_EVT_UNKNOWN,
	WIFI_EVT_CONNECTED,
	WIFI_EVT_DISCONNECTED,
	WIFI_EVT_SCAN_RESULT,
	WIFI_EVT_SCAN_DONE,
	WIFI_EVT_STARTED,
	WIFI_EVT_STOPPED,
};

enum wifi_frequency_band {
	WIFI_FREQ_2_4_GHZ,
	WIFI_FREQ_5_GHZ,
	WIFI_FREQ_6_GHZ,
};

enum wifi_security {
	WIFI_SEC_TYPE_UNKNOWN,
	WIFI_SEC_TYPE_NONE,
	WIFI_SEC_TYPE_WEP,
	WIFI_SEC_TYPE_PSK,
	WIFI_SEC_TYPE_PSK_SHA256,
	WIFI_SEC_TYPE_PSK_SAE,
};

enum wifi_mfp {
	WIFI_MFP_OPTIONAL,
	WIFI_MFP_REQUIRED,
	WIFI_MFP_DISABLED,
};

struct wifi_conn_param {
	const uint8_t *ssid;
	uint8_t ssid_len;
	const uint8_t *psk;
	uint8_t psk_len;
	const uint8_t *sae;
	uint8_t sae_len;

	enum wifi_frequency_band band;
	uint8_t channel;
	enum wifi_security security;
	enum wifi_mfp mfp;

	int timeout_ms;
};

struct wifi_iface_info {
	uint8_t ssid[WIFI_SSID_MAX_LEN];
	uint8_t ssid_len;

	uint8_t mac[WIFI_MAC_ADDR_LEN];
	uint8_t mac_len;

	uint8_t channel;
	int8_t rssi;
	enum wifi_security security;
};

struct wifi_scan_result {
	uint8_t ssid[WIFI_SSID_MAX_LEN];
	uint8_t ssid_len;

	enum wifi_frequency_band band;
	uint8_t channel;
	enum wifi_security security;
	enum wifi_mfp mfp;
	int8_t rssi;

	uint8_t mac[WIFI_MAC_ADDR_LEN];
	uint8_t mac_len;
};

struct wifi;
typedef void (*wifi_event_callback_t)(const struct wifi *iface,
				    enum wifi_event evt, const void *data);

struct wifi_api {
	int (*connect)(struct wifi *self, const struct wifi_conn_param *param);
	int (*disconnect)(struct wifi *self);
	int (*scan)(struct wifi *self);
	int (*start)(struct wifi *self);
	int (*stop)(struct wifi *self);
	int (*get_status)(struct wifi *self, struct wifi_iface_info *info);
	int (*register_event_callback)(struct wifi *self,
				const wifi_event_callback_t cb);
#if 0 /* TODO: define below interfaces */
	int (*get_stats)(struct wifi *self, struct wifi_stats *stats);
	int (*start_ap)();
	int (*stop_ap)();
#endif
};

struct wifi {
	struct wifi_api api;
	wifi_event_callback_t callback;

	struct wifi_iface_info status;

	struct {
		uint8_t v4[4];
		uint32_t v6[4];
	} ip;
};

#include "drivers/net/wifi_interface.h"

#if defined(__cplusplus)
}
#endif

#endif /* FPL_WIFI_H */
