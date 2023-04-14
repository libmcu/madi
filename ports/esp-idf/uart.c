/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/port/uart.h"
#include "libmcu/metrics.h"

#include <errno.h>
#include <string.h>

#include "driver/uart.h"

#define MAX_UART			3

#define DEFAULT_BUFSIZE			256
#define DEFAULT_RXQUEUE_LEN		8

struct uart {
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

	if (data_len == 1 && ((const char *)data)[0] == '\n') {
		if (uart_write_bytes(self->channel, " \b\n", 3) < 0) {
			metrics_increase(UARTErrorWrite);
			return -EIO;
		}
		return 1;
	}

	int written = uart_write_bytes(self->channel, data, data_len);

	if (written < 0) {
		metrics_increase(UARTErrorWrite);
		return -EIO;
	}

	return written;
}

int uart_port_read(struct uart *self, void *buf, size_t bufsize)
{
	if (!self || !self->activated) {
		metrics_increase(UARTErrorPipe);
		return -EPIPE;
	}

	int len = uart_read_bytes(self->channel,
			   buf, bufsize, (TickType_t)portMAX_DELAY);

	if (len < 0) {
		metrics_increase(UARTErrorRead);
		return -EIO;
	}

	return len;
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

	uart_config_t uart_config = {
		.baud_rate = baudrate,
		.data_bits = UART_DATA_8_BITS,
		.parity    = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
		.source_clk = UART_SCLK_DEFAULT,
#endif
	};

	if (uart_driver_install(self->channel, DEFAULT_BUFSIZE*2,
					DEFAULT_BUFSIZE*2, DEFAULT_RXQUEUE_LEN,
					NULL, 0) != ESP_OK) {
		metrics_increase(UARTFault);
		return -EFAULT;
	}

	self->baudrate = baudrate;
	self->activated = true;

	if (uart_param_config(self->channel, &uart_config) != ESP_OK) {
		metrics_increase(UARTErrorParam);
		return -EINVAL;
	}

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

	if (uart_driver_delete(self->channel) != ESP_OK) {
		metrics_increase(UARTFault);
		return -EFAULT;
	}

	self->activated = false;

	return 0;
}

struct uart *uart_port_create(uint8_t channel)
{
	static struct uart uart[MAX_UART];

	if (channel >= MAX_UART || uart[channel].activated) {
		return NULL;
	}

	uart[channel].channel = channel;

	return &uart[channel];
}

void uart_port_delete(struct uart *self)
{
	memset(self, 0, sizeof(*self));
}
