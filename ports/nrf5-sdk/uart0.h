/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NRF52_UART0_H
#define NRF52_UART0_H

#include <stddef.h>
#include <stdint.h>

void uart0_init(uint32_t baudrate);
int uart0_write(const void *data, size_t datasize);
int uart0_read(void *buf, size_t bufsize);

#endif
