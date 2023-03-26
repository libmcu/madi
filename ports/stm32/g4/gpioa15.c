/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ledind.h"
#include "gpio.h"

#define LED_GPIO_PORT			GPIOA
#define LED_GPIO_PIN			GPIO_PIN_15

static int set_led_level(bool on)
{
	if (on) {
		HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, 0);
	} else {
		HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, 1);
	}
}

static int toggle_led_level(void)
{
	HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
	return 0;
}

static int enable_led_gpio(bool enable)
{
	if (enable) {
		MX_GPIO_InitRecursive();
		set_led_level(0);
		return 0;
	}

	/* disable */
	set_led_level(0);
	/* TODO: Implement */
	return 0;
}

const struct ledind_api *ledind_gpio_create(void)
{
	static struct ledind_api api = {
		.enable = enable_led_gpio,
		.set = set_led_level,
		.toggle = toggle_led_level,
	};

	return &api;
}
