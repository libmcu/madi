/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "uart1.h"
#include "stm32f1xx_hal.h"

static UART_HandleTypeDef huart1;

static void initialize_gpio(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {
		.Pin = GPIO_PIN_9,
		.Mode = GPIO_MODE_AF_PP,
		.Speed = GPIO_SPEED_FREQ_HIGH,
	};
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static int initialize_uart1(uint32_t baudrate)
{
	initialize_gpio();

	__HAL_RCC_USART1_CLK_ENABLE();

	huart1.Instance = USART1;
	huart1.Init.BaudRate = baudrate;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	return HAL_UART_Init(&huart1);
}

static void deinitialize_gpio(void)
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
}

static void deinitialize_uart1(void)
{
	deinitialize_gpio();
	__HAL_RCC_USART1_CLK_DISABLE();
}

size_t uart1_write(void const *data, size_t datasize)
{
	return HAL_UART_Transmit(&huart1, data, (uint16_t)datasize, 100U)
			== HAL_OK? datasize : 0;
}

size_t uart1_read(void *buf, size_t bufsize)
{
	return HAL_UART_Receive(&huart1, buf, (uint16_t)bufsize, 0U)
			== HAL_OK? bufsize : 0;
}

void uart1_init(uint32_t baudrate)
{
	initialize_uart1(baudrate);
}
