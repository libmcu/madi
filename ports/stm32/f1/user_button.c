/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "user_button.h"
#include "gpio.h"

#define USER_BUTTON_GPIO_PORT			GPIOB
#define USER_BUTTON_GPIO_PIN			GPIO_PIN_11

static void (*dispatch_callback)(void);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin != USER_BUTTON_GPIO_PIN) {
		return;
	}

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
	dispatch_callback = event_callback;
}
