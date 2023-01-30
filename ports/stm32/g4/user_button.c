/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "user_button.h"
#include "gpio.h"

#define USER_BUTTON_GPIO_PORT			GPIOB
#define USER_BUTTON_GPIO_PIN			GPIO_PIN_8

static void (*dispatch_callback)(void);

static void on_gpio_state_change(void)
{
	if (dispatch_callback) {
		(*dispatch_callback)();
	}
}

int user_button_get_state(void)
{
	return !HAL_GPIO_ReadPin(USER_BUTTON_GPIO_PORT, USER_BUTTON_GPIO_PIN);
}

void user_button_init_hw(void (*event_callback)(void))
{
	MX_GPIO_InitRecursive();
	MX_GPIO_RegisterCallback(USER_BUTTON_GPIO_PIN, on_gpio_state_change);

	dispatch_callback = event_callback;
}
