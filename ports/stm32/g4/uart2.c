/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "uart2.h"
#include "usart.h"
#include "libmcu/metrics.h"

size_t uart2_write(void const *data, size_t datasize)
{
	if (HAL_UART_Transmit(&huart2, (const uint8_t *)data,
		       (uint16_t)datasize, 100U) != HAL_OK) {
		metrics_increase(UARTError);
		return 0;
	}

	return datasize;
}

size_t uart2_read(void *buf, size_t bufsize)
{
	if (HAL_UART_Receive(&huart2, (uint8_t *)buf, (uint16_t)bufsize,
			HAL_MAX_DELAY) != HAL_OK) {
		metrics_increase(UARTError);
		return 0;
	}

	/* FIXME: it should return the number of bytes received */
	return bufsize;
}

int uart2_enable(uint32_t baudrate)
{
	(void)baudrate;
	MX_USART2_UART_Init();
	return 0;
}

int uart2_disable(void)
{
	MX_USART2_UART_DeInit();
	return 0;
}
