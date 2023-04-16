/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "battery.h"
#include "libmcu/adc.h"
#include "gpio.h"

#define MONITOR_ENABLE_GPIO_PORT	GPIOB
#define MONITOR_ENABLE_GPIO_PIN		GPIO_PIN_2
#define MONITOR_INTR_GPIO_PIN		GPIO_PIN_6 /* GPIOC */
#define ADC_CHANNEL			ADC_CH_5 /* PC.4 ADC2.5 */

static struct adc *adc;
static void (*dispatch_callback)(void);

static void on_interrupt(void)
{
	if (dispatch_callback) {
		(*dispatch_callback)();
	}
}

static void initialize_monitor_gpio(void)
{
	MX_GPIO_InitRecursive();
	MX_GPIO_RegisterCallback(MONITOR_INTR_GPIO_PIN, on_interrupt);
}

static int enable_monitor(bool enable)
{
	if (enable) {
		adc_enable(adc);
	} else {
		adc_disable(adc);
	}

	if (enable) {
		HAL_GPIO_WritePin(MONITOR_ENABLE_GPIO_PORT,
				MONITOR_ENABLE_GPIO_PIN, 1);

		adc_calibrate(adc);
		adc_channel_init(adc, ADC_CHANNEL);
	} else {
		HAL_GPIO_WritePin(MONITOR_ENABLE_GPIO_PORT,
				MONITOR_ENABLE_GPIO_PIN, 0);
	}

	return 0;
}

static int get_level_adc(void)
{
	adc_measure(adc);
	return adc_read(adc, ADC_CHANNEL);
}

static int adc_to_millivolts(int raw)
{
	return adc_convert_to_millivolts(adc, raw);
}

struct battery_monitor *battery_monitor_init(void (*on_event_callback)(void))
{
	static struct battery_monitor mon = {
		.enable = enable_monitor,
		.get_level_adc = get_level_adc,
		.adc_to_millivolts = adc_to_millivolts,
	};

	initialize_monitor_gpio();
	dispatch_callback = on_event_callback;

	if ((adc = adc_create(2)) == NULL) {
		return NULL;
	}

	return &mon;
}
