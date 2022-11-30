/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_WIFI_WRAPPER_H
#define FPL_WIFI_WRAPPER_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "drivers/net/wifi.h"
#include "util.h"

#if defined(WIFI_DEFAULT_INTERFACE)
#define wifi_create_default	CONCAT(WIFI_DEFAULT_INTERFACE, _wifi_create)
#define wifi_destroy_default	CONCAT(WIFI_DEFAULT_INTERFACE, _wifi_destroy)

struct wifi *wifi_create_default(void);
void wifi_destroy_default(struct wifi *inst);
#else
#define wifi_create_default()	0
#define wifi_destroy_default(x)
#endif

#if defined(__cplusplus)
}
#endif

#endif /* FPL_WIFI_WRAPPER_H */
