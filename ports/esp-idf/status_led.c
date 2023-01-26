/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "status_led.h"
#include "driver/gpio.h"

#define LED_GPIO_NUMBER			35

void status_led_on(void)
{
	gpio_set_level(LED_GPIO_NUMBER, 0);
}

void status_led_off(void)
{
	gpio_set_level(LED_GPIO_NUMBER, 1);
}

void status_led_toggle(void)
{
	static int val;
	gpio_set_level(LED_GPIO_NUMBER, val);
	val ^= 1;
}

void status_led_init(void)
{
	gpio_config_t io_conf = {
		.intr_type = GPIO_INTR_DISABLE,
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = (1ULL << LED_GPIO_NUMBER),
	};
	gpio_config(&io_conf);
}
