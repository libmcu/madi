/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBMCU_USBD_H
#define LIBMCU_USBD_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef void (*usbd_periodic_callback_t)(void *ctx);

int usbd_port_init(void);

int usbd_init(void);
int usbd_enable(void);
int usbd_disable(void);
int usbd_register_periodic_callback(usbd_periodic_callback_t cb, void *arg);

void usbd_cdc_acm_init(void);
int usbd_cdc_acm_read(void *buf, size_t bufsize);
int usbd_cdc_acm_write(const void *data, size_t datasize);

typedef int (*usbd_cdc_net_tx_handler_t)(const void *p, void *q);
typedef int (*usbd_cdc_net_rx_handler_t)(const uint8_t *data, uint16_t datasize);

void usbd_cdc_net_init(void);
void usbd_cdc_net_register_rx_handler(usbd_cdc_net_rx_handler_t handler);
int usbd_cdc_net_send(const void *data, uint16_t datasize,
		usbd_cdc_net_tx_handler_t cb);

#if defined(__cplusplus)
}
#endif

#endif
