/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ledind.h"
#include "nrf_gpio.h"

#define LED_GPIO_NUMBER			20 /*P0.20*/

static int set_led_level(bool on)
{
	if (on) {
		nrf_gpio_pin_clear(LED_GPIO_NUMBER);
	} else {
		nrf_gpio_pin_set(LED_GPIO_NUMBER);
	}
}

static int toggle_led_level(void)
{
	nrf_gpio_pin_toggle(LED_GPIO_NUMBER);
	return 0;
}

static int enable_led_gpio(bool enable)
{
	if (enable) {
		nrf_gpio_cfg_output(LED_GPIO_NUMBER);
		return 0;
	}

	/* disable */
	nrf_gpio_cfg_default(LED_GPIO_NUMBER);
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
