/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_UART0_H
#define ESP_UART0_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

void uart0_init(uint32_t baudrate);
int uart0_write_async(void const *data, size_t datasize);
int uart0_read(void *buf, size_t bufsize);

#if defined(__cplusplus)
}
#endif

#endif
