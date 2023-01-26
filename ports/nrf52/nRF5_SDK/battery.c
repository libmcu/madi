/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "battery.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"
#include "adc.h"
#include "bq25180.h"

#define MONITOR_ENABLE_GPIO_NUMBER		27 /* P0.27 */
#define MONITOR_INTR_GPIO_NUMBER		26 /* P0.26 */
#define ADC_CHANNEL				 7 /* P0.31, AIN7 */

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

static int calc_average(const int *samples, int n)
{
	int sum = 0;

	for (int i = 0; i < n; i++) {
		sum += samples[i];
	}

	return sum / n;
}

static int calc_variance(const int *samples, int n, int avg)
{
	int sum = 0;

	for (int i = 0; i < n; i++) {
		int diff = samples[i] - avg;
		sum += diff * diff;
	}

	return sum / n;
}

static int calc_average_filtered(const int *samples, int n, int avg, int var)
{
	int c = 0;
	int sum = 0;

	for (int i = 0; i < n; i++) {
		int diff = samples[i] - avg;

		if ((diff * diff) > var) {
			continue;
		}

		sum += samples[i];
		c++;
	}

	return sum / c;
}

int battery_enable_monitor(bool enable)
{
	if (enable) {
		nrf_gpio_pin_set(MONITOR_ENABLE_GPIO_NUMBER);
	} else {
		nrf_gpio_pin_clear(MONITOR_ENABLE_GPIO_NUMBER);
	}

	return 0;
}

int battery_level_raw(void)
{
	if (!adc) {
		return 0;
	}

#define NR_SAMPLES	20
	int samples[NR_SAMPLES];

	adc_get_raw(adc, ADC_CHANNEL); /* The first one always to be outlier. */
	for (int i = 0; i < NR_SAMPLES; i++) {
		samples[i] = adc_get_raw(adc, ADC_CHANNEL);
	}

	int avg = calc_average(samples, NR_SAMPLES);
	int var = calc_variance(samples, NR_SAMPLES, avg);
	avg = calc_average_filtered(samples, NR_SAMPLES, avg, var);

	return avg;
}

int battery_raw_to_millivolts(int raw)
{
	if (!adc) {
		return 0;
	}

	int mv_raw = adc_raw_to_millivolts(adc, raw);
	return mv_raw * 1000/128/*scale*/ - 54/*offset*/;
}

int battery_init(void (*on_event_callback)(void))
{
	adc = adc_create();
	adc_enable(adc, true);
	adc_calibrate(adc);
	adc_channel_init(ADC_CHANNEL);

	initialize_monitor_gpio();

	dispatch_callback = on_event_callback;

	bq25180_reset(false);
	bq25180_enable_interrupt(BQ25180_INTR_CHARGING_STATUS);
	bq25180_set_watchdog_timer(BQ25180_WDT_DISABLE);
	bq25180_enable_thermal_protection(false);
	bq25180_enable_vindpm(BQ25180_VINDPM_4700mV);
	bq25180_set_sys_voltage(BQ25180_SYS_REG_PASS_THROUGH);

	return 0;
}
