/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBMCU_USBD_CDC_H
#define LIBMCU_USBD_CDC_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

int usbd_port_init(void);

int usbd_init(void);
int usbd_enable(void);
int usbd_disable(void);

void usbd_cdc_acm_init(void);
void usbd_cdc_acm_step(void);
int usbd_cdc_acm_read(void *buf, size_t bufsize);
int usbd_cdc_acm_write(const void *data, size_t datasize);

void usbd_cdc_net_init(void);
void usbd_cdc_net_step(void);
int usbd_cdc_net_send(const void *data, uint16_t datasize);
int usbd_cdc_net_send_post(const void *p, void *q);
int usbd_cdc_net_received(const void *data, uint16_t datasize);
void usbd_cdc_net_received_post(void);
void usbd_cdc_net_get_mac(uint8_t mac[6]);

#if defined(__cplusplus)
}
#endif

#endif
