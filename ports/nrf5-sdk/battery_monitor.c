/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "battery.h"
#include "libmcu/adc.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#define MONITOR_ENABLE_GPIO_NUMBER		27 /* P0.27 */
#define MONITOR_INTR_GPIO_NUMBER		26 /* P0.26 */
#define ADC_CHANNEL				ADC_CH_7 /* P0.31, AIN7 */

static struct adc *adc;
static void (*dispatch_callback)(void);

static void on_interrupt(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	(void)pin;
	(void)action;

	if (dispatch_callback) {
		(*dispatch_callback)();
	}
}

static void initialize_monitor_gpio(void)
{
	nrf_gpio_cfg_output(MONITOR_ENABLE_GPIO_NUMBER);

	nrfx_gpiote_in_config_t cfg = {
		.pull = NRF_GPIO_PIN_PULLUP,
		.sense = GPIOTE_CONFIG_POLARITY_HiToLo,
	};
	if (nrfx_gpiote_in_init(MONITOR_INTR_GPIO_NUMBER, &cfg, on_interrupt)
			!= NRF_SUCCESS) {
	}

	nrfx_gpiote_in_event_enable(MONITOR_INTR_GPIO_NUMBER, true);
}

static int enable_monitor(bool enable)
{
	if (enable) {
		adc_enable(adc);
	} else {
		adc_disable(adc);
	}

	if (enable) {
		nrf_gpio_pin_set(MONITOR_ENABLE_GPIO_NUMBER);

		adc_calibrate(adc);
		adc_channel_init(adc, ADC_CHANNEL);
	} else {
		nrf_gpio_pin_clear(MONITOR_ENABLE_GPIO_NUMBER);
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

	if ((adc = adc_create(0)) == NULL) {
		return NULL;
	}

	return &mon;
}
