/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "userbutton.h"
#include "gpio.h"

#define USERBUTTON_GPIO_PORT			GPIOB
#define USERBUTTON_GPIO_PIN			GPIO_PIN_11

static void (*dispatch_callback)(void);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin != USERBUTTON_GPIO_PIN) {
		return;
	}

	if (dispatch_callback) {
		(*dispatch_callback)();
	}
}

static int userbutton_get_state(void)
{
	return !HAL_GPIO_ReadPin(USERBUTTON_GPIO_PORT, USERBUTTON_GPIO_PIN);
}

userbutton_read_state_func_t userbutton_gpio_init(
		void (*event_callback)(void))
{
	MX_GPIO_InitRecursive();
	dispatch_callback = event_callback;

	return userbutton_get_state;
}
