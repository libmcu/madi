/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef STM32G4_UART2_H
#define STM32G4_UART2_H

#include <stddef.h>
#include <stdint.h>

int uart2_enable(uint32_t baudrate);
int uart2_disable(void);
size_t uart2_write(void const *data, size_t datasize);
size_t uart2_read(void *buf, size_t bufsize);

#endif
