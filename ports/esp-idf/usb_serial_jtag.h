/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_USB_SERIAL_JTAG_H
#define ESP_USB_SERIAL_JTAG_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>

void serial_jtag_init(void);
size_t serial_jtag_write_async(void const *data, size_t datasize);
size_t serial_jtag_read(void *buf, size_t bufsize);

#if defined(__cplusplus)
}
#endif

#endif
