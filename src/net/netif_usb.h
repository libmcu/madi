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

void *netif_usb_create(const struct net_iface_param *param);
void netif_usb_delete(void *p);
int netif_usb_input(const void *data, uint16_t datasize);
int netif_usb_clean_input(bool xfer);
void netif_usb_input_done(void);
int netif_usb_output(const void *data, uint16_t datasize);
int netif_usb_clean_output(const void *p, void *q);
int netif_usb_step(void);

#if defined(__cplusplus)
}
#endif

#endif /* NETIF_USB_H */
