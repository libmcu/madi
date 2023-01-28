/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LIBMCU_UART_INTERFACE_H
#define LIBMCU_UART_INTERFACE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "puart/uart.h"

static inline int uart_write(struct uart *self, const void *data, size_t data_len)
{
	return ((struct uart_api *)self)->write(self, data, data_len);
}

static inline int uart_read(struct uart *self, void *buf, size_t bufsize)
{
	return ((struct uart_api *)self)->read(self, buf, bufsize);
}

static inline int uart_enable(struct uart *self, uint32_t baudrate)
{
	return ((struct uart_api *)self)->enable(self, baudrate);
}

static inline int uart_disable(struct uart *self)
{
	return ((struct uart_api *)self)->disable(self);
}

#if defined(__cplusplus)
}
#endif

#endif /* LIBMCU_UART_INTERFACE_H */
