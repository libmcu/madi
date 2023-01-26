/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "status_led.h"
#include "nrf_gpio.h"

#define LED_GPIO_NUMBER			20 /*P0.20*/

void status_led_on(void)
{
	nrf_gpio_pin_clear(LED_GPIO_NUMBER);
}

void status_led_off(void)
{
	nrf_gpio_pin_set(LED_GPIO_NUMBER);
}

void status_led_toggle(void)
{
	nrf_gpio_pin_toggle(LED_GPIO_NUMBER);
}

void status_led_init(void)
{
	nrf_gpio_cfg_output(LED_GPIO_NUMBER);
}
