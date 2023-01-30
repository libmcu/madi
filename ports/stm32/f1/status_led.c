/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "status_led.h"
#include "gpio.h"

#define LED_GPIO_PORT			GPIOC
#define LED_GPIO_PIN			GPIO_PIN_13

void status_led_on(void)
{
	HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, 0);
}

void status_led_off(void)
{
	HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, 1);
}

void status_led_toggle(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
}

void status_led_init(void)
{
	MX_GPIO_InitRecursive();
}
