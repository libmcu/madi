/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/port/uart.h"
#include "libmcu/metrics.h"

#include <errno.h>
#include <string.h>

#include "usart.h"

#define MAX_UART			1

struct uart {
	UART_HandleTypeDef *handle;
	uint8_t channel;
	uint32_t baudrate;
	bool activated;
};

int uart_port_write(struct uart *self, const void *data, size_t data_len)
{
	if (!self || !self->activated) {
		metrics_increase(UARTErrorPipe);
		return -EPIPE;
	}

	if (HAL_UART_Transmit(self->handle, (const uint8_t *)data,
		       (uint16_t)data_len, 100U) != HAL_OK) {
		metrics_increase(UARTErrorWrite);
		return -EIO;
	}

	return (int)data_len;
}

int uart_port_read(struct uart *self, void *buf, size_t bufsize)
{
	if (!self || !self->activated) {
		metrics_increase(UARTErrorPipe);
		return -EPIPE;
	}

	if (HAL_UART_Receive(self->handle, (uint8_t *)buf, (uint16_t)bufsize,
			HAL_MAX_DELAY) != HAL_OK) {
		metrics_increase(UARTErrorRead);
		return -EIO;
	}

	/* FIXME: it should return the number of bytes received */
	return (int)bufsize;
}

int uart_port_enable(struct uart *self, uint32_t baudrate)
{
	if (!self) {
		metrics_increase(UARTErrorPipe);
		return -EPIPE;
	} else if (self->activated) {
		metrics_increase(UARTErrorSequence);
		return -EALREADY;
	}

	MX_USART2_UART_Init();

	self->baudrate = baudrate;
	self->activated = true;

	return 0;
}

int uart_port_disable(struct uart *self)
{
	if (!self) {
		metrics_increase(UARTErrorPipe);
		return -EPIPE;
	} else if (!self->activated) {
		metrics_increase(UARTErrorSequence);
		return -EALREADY;
	}

	MX_USART2_UART_DeInit();

	self->activated = false;

	return 0;
}

struct uart *uart_port_create(uint8_t channel)
{
	static struct uart uart[MAX_UART];

	if (channel != 2 || uart[0].activated) {
		return NULL;
	}

	uart[0].channel = channel;
	uart[0].handle = &huart2;

	return &uart[0];
}

void uart_port_delete(struct uart *self)
{
	memset(self, 0, sizeof(*self));
}
