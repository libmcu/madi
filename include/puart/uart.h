/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LIBMCU_UART_H
#define LIBMCU_UART_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

struct uart;

struct uart_api {
	int (*read)(struct uart *self, void *buf, size_t bufsize);
	int (*write)(struct uart *self, const void *data, size_t data_len);
	int (*enable)(struct uart *self, uint32_t baudrate);
	int (*disable)(struct uart *self);
};

#include "puart/uart_interface.h"

#if defined(__cplusplus)
}
#endif

#endif /* LIBMCU_UART_H */
