/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "user_button.h"
#include "libmcu/assert.h"
#include "nrf_gpio.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_gpiote.h"

#define USER_BUTTON_GPIO_NUMBER		39 /*P1.07*/

static void (*dispatch_callback)(void);

static void on_gpio_state_change(nrf_drv_gpiote_pin_t pin,
		nrf_gpiote_polarity_t action)
{
	if (pin != USER_BUTTON_GPIO_NUMBER) {
		return;
	}

	if (dispatch_callback) {
		(*dispatch_callback)();
	}
}

static int user_button_get_state(void)
{
	return !nrf_gpio_pin_read(USER_BUTTON_GPIO_NUMBER);
}

user_button_read_state_func_t user_button_gpio_init(
		void (*event_callback)(void))
{
	nrf_drv_gpiote_in_config_t cfg = {
		.pull = NRF_GPIO_PIN_PULLUP,
		.sense = GPIOTE_CONFIG_POLARITY_HiToLo,
	};

	if (nrf_drv_gpiote_in_init(USER_BUTTON_GPIO_NUMBER, &cfg,
			on_gpio_state_change) != NRF_SUCCESS) {
		assert(0);
	}

	nrf_drv_gpiote_in_event_enable(USER_BUTTON_GPIO_NUMBER, true);

	dispatch_callback = event_callback;

	return user_button_get_state;
}
