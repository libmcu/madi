/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBMCU_NETIF_USB_H
#define LIBMCU_NETIF_USB_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

struct netif_param {
	uint32_t ip;
	uint32_t netmask;
	uint32_t gateway;
};

struct netif;
struct netif *netif_create_usb(const struct netif_param *param);
void netif_delete_usb(void *p);

#if defined(__cplusplus)
}
#endif

#endif
