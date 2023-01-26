/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "user_button.h"
#include "driver/gpio.h"
#include "esp_attr.h"

#define USER_BUTTON_GPIO_NUMBER		0

static void (*dispatch_callback)(void);

static void IRAM_ATTR on_gpio_state_change(void *arg)
{
	(void)arg;

	if (dispatch_callback) {
		(*dispatch_callback)();
	}
}

int user_button_get_state(void)
{
	return !gpio_get_level(USER_BUTTON_GPIO_NUMBER);
}

void user_button_init_hw(void (*event_callback)(void))
{
	gpio_config_t io_conf = {
		.intr_type = GPIO_INTR_NEGEDGE,
		.mode = GPIO_MODE_INPUT,
		.pin_bit_mask = (1ULL << USER_BUTTON_GPIO_NUMBER),
	};
	gpio_config(&io_conf);

	gpio_isr_handler_add(USER_BUTTON_GPIO_NUMBER, on_gpio_state_change, 0);

	dispatch_callback = event_callback;
}
