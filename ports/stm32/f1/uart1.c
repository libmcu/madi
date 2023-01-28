/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "uart1.h"
#include "usart.h"
#include "libmcu/metrics.h"

size_t uart1_write(void const *data, size_t datasize)
{
	if (HAL_UART_Transmit(&huart1, (const uint8_t *)data,
		       (uint16_t)datasize, 100U) != HAL_OK) {
		return 0;
	}

	return datasize;
}

size_t uart1_read(void *buf, size_t bufsize)
{
	if (HAL_UART_Receive(&huart1, (uint8_t *)buf, (uint16_t)bufsize,
			HAL_MAX_DELAY) != HAL_OK) {
		return 0;
	}

	/* FIXME: it should return the number of bytes received */
	return bufsize;
}

int uart1_enable(uint32_t baudrate)
{
	(void)baudrate;
	MX_USART1_UART_Init();
	return 0;
}

int uart1_disable(void)
{
	MX_USART1_UART_DeInit();
	return 0;
}
