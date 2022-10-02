/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_WIFI_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

#define WIFI_SSID_MAX_LEN		32
#define WIFI_MAC_ADDR_LEN		6

typedef struct wifi_iface * wifi_iface_t;

enum wifi_event {
	WIFI_EVT_UNKNOWN,
	WIFI_EVT_CONNECTED,
	WIFI_EVT_DISCONNECTED,
	WIFI_EVT_SCAN_RESULT,
	WIFI_EVT_SCAN_DONE,
	WIFI_EVT_STARTED,
};

enum wifi_mode {
	WIFI_MODE_INFRA,
	WIFI_MODE_ACCESS_POINT,
	WIFI_MODE_MESH,
};

enum wifi_state {
	WIFI_STATE_UNKNOWN,
	WIFI_STATE_DISABLED,
	WIFI_STATE_DISCONNECTING,
	WIFI_STATE_DISCONNECTED,
	WIFI_STATE_INACTIVE,
	WIFI_STATE_SCANNING,
	WIFI_STATE_AUTHENTICATING,
	WIFI_STATE_ASSOCIATING,
	WIFI_STATE_ASSOCIATED,
};

enum wifi_security {
	WIFI_SEC_TYPE_UNKNOWN,
	WIFI_SEC_TYPE_NONE,
	WIFI_SEC_TYPE_WEP,
	WIFI_SEC_TYPE_PSK,
	WIFI_SEC_TYPE_PSK_SHA256,
	WIFI_SEC_TYPE_PSK_SAE,
};

enum wifi_frequency_band {
	WIFI_FREQ_2_4_GHZ,
	WIFI_FREQ_5_GHZ,
	WIFI_FREQ_6_GHZ,
};

enum wifi_mfp {
	WIFI_MFP_DISABLED,
	WIFI_MFP_OPTIONAL,
	WIFI_MFP_REQUIRED,
};

struct wifi_conf {
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

struct wifi_ap_info {
	uint8_t ssid[WIFI_SSID_MAX_LEN];
	uint8_t ssid_len;

	uint8_t mac[WIFI_MAC_ADDR_LEN];
	uint8_t mac_len;

	uint8_t channel;
	int8_t rssi;
	enum wifi_security security;
};

typedef void (*wifi_event_callback_t)(const wifi_iface_t iface,
				    enum wifi_event evt, const void *data);

int wifi_connect(wifi_iface_t iface, const struct wifi_conf *param);
int wifi_disconnect(wifi_iface_t iface);
int wifi_scan(wifi_iface_t iface);
int wifi_get_ap_info(wifi_iface_t iface, struct wifi_ap_info *info);

wifi_iface_t wifi_create(void);
int wifi_register_event_callback(wifi_iface_t iface,
				 const wifi_event_callback_t cb);

void wifi_raise_event(wifi_iface_t iface, enum wifi_event evt);
void wifi_raise_event_with_data(wifi_iface_t iface,
				enum wifi_event evt, const void *data);

void wifi_set_state(wifi_iface_t iface, enum wifi_state state);
void wifi_set_mode(wifi_iface_t iface, enum wifi_mode mode);

#if 0
int wifi_add_event_callback(wifi_iface_t iface,
			    enum wifi_event evt,
			    const wifi_event_callback_t cb);
wifi_enable_ap()
wifi_disable_ap()
#endif

#include "common/wifi_private.h"

#if defined(__cplusplus)
}
#endif

#endif /* FPL_WIFI_H */
