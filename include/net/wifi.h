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

#include "net/wifi_interface.h"
#include "net/util.h"

struct wifi {
	struct wifi_interface api;
	wifi_event_callback_t callback;

	struct wifi_iface_info status;

	struct {
		uint8_t v4[4];
		uint32_t v6[4];
	} ip;
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

static inline int wifi_connect(struct wifi *iface,
			       const struct wifi_conn_param *param)
{
	return iface->api.connect(iface, param);
}

static inline int wifi_disconnect(struct wifi *iface)
{
	return iface->api.disconnect(iface);
}

static inline int wifi_scan(struct wifi *iface)
{
	return iface->api.scan(iface);
}

static inline int wifi_start(struct wifi *iface)
{
	return iface->api.start(iface);
}

static inline int wifi_stop(struct wifi *iface)
{
	return iface->api.stop(iface);
}

static inline int wifi_get_status(struct wifi *iface,
				  struct wifi_iface_info *info)
{
	return iface->api.get_status(iface, info);
}

static inline int wifi_register_event_callback(struct wifi *iface,
					       const wifi_event_callback_t cb)
{
	return iface->api.register_event_callback(iface, cb);
}

#if defined(WIFI_DEFAULT_INTERFACE)
#define wifi_create_default	CONCAT(WIFI_DEFAULT_INTERFACE, _wifi_create)
#define wifi_destroy_default	CONCAT(WIFI_DEFAULT_INTERFACE, _wifi_destroy)
#endif

struct wifi *wifi_create_default(void);
void wifi_destroy_default(struct wifi *iface);

#if defined(__cplusplus)
}
#endif

#endif /* FPL_WIFI_H */
