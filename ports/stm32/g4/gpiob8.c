/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "userbutton.h"
#include "gpio.h"

#define USERBUTTON_GPIO_PORT			GPIOB
#define USERBUTTON_GPIO_PIN			GPIO_PIN_8

static void (*dispatch_callback)(void);

static void on_gpio_state_change(void)
{
	if (dispatch_callback) {
		(*dispatch_callback)();
	}
}

static int userbutton_get_state(void)
{
	return HAL_GPIO_ReadPin(USERBUTTON_GPIO_PORT, USERBUTTON_GPIO_PIN);
}

userbutton_read_state_func_t userbutton_gpio_init(
		void (*event_callback)(void))
{
	MX_GPIO_InitRecursive();
	MX_GPIO_RegisterCallback(USERBUTTON_GPIO_PIN, on_gpio_state_change);

	dispatch_callback = event_callback;

	return userbutton_get_state;
}
