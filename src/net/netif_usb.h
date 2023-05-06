/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NETIF_USB_H
#define NETIF_USB_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "net.h"
#include "pusb/usbd.h"

void *netif_usb_create(const struct net_iface_param *param);
void netif_usb_delete(void *p);

#if defined(__cplusplus)
}
#endif

#endif /* NETIF_USB_H */
