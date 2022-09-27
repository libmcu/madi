/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_WIFI_PRIVATE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

struct wifi_iface {
	volatile uint8_t state; /**< @ref wifi_iface_state */
	volatile uint8_t state_prev;
	volatile uint8_t mode;  /**< @ref wifi_mode */
	volatile uint8_t mode_prev;

	wifi_event_callback_t callbacks;

	uint8_t mac[WIFI_MAC_ADDR_LEN];
	int8_t rssi;
	int8_t padding;

	struct {
		uint8_t v4[4];
		uint32_t v6[4];
	} ip;
};

#if defined(__cplusplus)
}
#endif

#endif /* FPL_WIFI_PRIVATE_H */
