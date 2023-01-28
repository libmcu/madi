/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef STM32F1_UART1_H
#define STM32F1_UART1_H

#include <stddef.h>
#include <stdint.h>

int uart1_enable(uint32_t baudrate);
int uart1_disable(void);
size_t uart1_write(void const *data, size_t datasize);
size_t uart1_read(void *buf, size_t bufsize);

#endif
