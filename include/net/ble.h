/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_BLE_WRAPPER_H
#define FPL_BLE_WRAPPER_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "drivers/net/ble.h"
#include "util.h"

#if defined(BLE_DEFAULT_INTERFACE)
#define ble_create_default	CONCAT(BLE_DEFAULT_INTERFACE, _ble_create)
#define ble_destroy_default	CONCAT(BLE_DEFAULT_INTERFACE, _ble_destroy)

struct ble *ble_create_default(void);
void ble_destroy_default(struct ble *inst);
#else
#define ble_create_default()	0
#define ble_destroy_default(x)
#endif

#if defined(__cplusplus)
}
#endif

#endif /* FPL_BLE_WRAPPER_H */
