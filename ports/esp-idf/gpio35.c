/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ledind.h"
#include "driver/gpio.h"

#define LED_GPIO_NUMBER			35

static int toggle_level;

static int set_led_level(bool on)
{
	toggle_level = (int)on;
	return gpio_set_level(LED_GPIO_NUMBER, !on) * -1;
}

static int toggle_led_level(void)
{
	toggle_level ^= 1;
	return gpio_set_level(LED_GPIO_NUMBER, !toggle_level) * -1;
}

static int enable_led_gpio(bool enable)
{
	if (enable) {
		gpio_config_t io_conf = {
			.intr_type = GPIO_INTR_DISABLE,
			.mode = GPIO_MODE_OUTPUT,
			.pin_bit_mask = (1ULL << LED_GPIO_NUMBER),
		};
		return gpio_config(&io_conf) * -1;
	}

	/* disable */
	return gpio_reset_pin(LED_GPIO_NUMBER) * -1;
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
