/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "uart0.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUFSIZE				256
#define RXQUEUE_LEN			8

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
		     RXQUEUE_LEN, NULL, 0);
	uart_param_config(UART_NUM_0, &uart_config);
}

size_t uart0_write_async(void const *data, size_t datasize)
{
	if (datasize == 1 && ((const char *)data)[0] == '\n') {
		uart_write_bytes(UART_NUM_0, " \b\n", 3);
		return 1;
	}

	int written = uart_write_bytes(UART_NUM_0, data, datasize);
	return written > 0 ? (size_t)written : 0;
}

size_t uart0_read(void *buf, size_t bufsize)
{
	int len = uart_read_bytes(UART_NUM_0,
			   buf, bufsize, (TickType_t)portMAX_DELAY);
	return len > 0 ? len : 0;
}
