/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "uart0.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define BUFSIZE				256
#define RXQUEUE_LEN			8

static QueueHandle_t rxqueue;

void uart0_init(uint32_t baudrate)
{
	uart_config_t uart_config = {
		.baud_rate = baudrate,
		.data_bits = UART_DATA_8_BITS,
		.parity    = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	};

	uart_driver_install(UART_NUM_0, BUFSIZE*2, BUFSIZE*2,
		     RXQUEUE_LEN, &rxqueue, 0);
	uart_param_config(UART_NUM_0, &uart_config);
}

size_t uart0_write_async(void const *data, size_t datasize)
{
	int written = uart_write_bytes(UART_NUM_0, data, datasize);

	return written > 0 ? (size_t)written : 0;
}

size_t uart0_read(void *buf, size_t bufsize)
{
	uart_event_t evt;
	int len = 0;

	xQueueReceive(rxqueue, (void *)&evt, (TickType_t)portMAX_DELAY);

	switch (evt.type) {
	case UART_DATA:
		len = uart_read_bytes(UART_NUM_0, buf, bufsize, 0);
		len = len > 0 ? len : 0;
		break;
	default:
		break;
	}

	return (size_t)len;
}
