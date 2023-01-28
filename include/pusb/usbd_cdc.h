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

int usbd_cdc_init(void);
int usbd_cdc_enable(void);
int usbd_cdc_disable(void);
size_t usbd_cdc_write(const void *data, size_t datasize);
size_t usbd_cdc_read(void *buf, size_t bufsize);

#if defined(__cplusplus)
}
#endif

#endif
