/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "battery.h"
#include "adc1.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "bq25180.h"

#define MONITOR_ENABLE_GPIO_NUMBER		4
#define MONITOR_GPIO_NUMBER			7 /* ADC1_6 */
#define MONITOR_INTR_GPIO_NUMBER		14
#define ADC_CHANNEL				6

static struct adc *adc;
static void (*dispatch_callback)(void);

static void IRAM_ATTR on_interrupt(void *arg)
{
	if (dispatch_callback) {
		(*dispatch_callback)();
	}
}

static void initialize_monitor_gpio(void)
{
	gpio_config_t io_conf = {
		.intr_type = GPIO_INTR_DISABLE,
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = (1ULL << MONITOR_ENABLE_GPIO_NUMBER),
	};
	gpio_config(&io_conf);

	/* bq25180 interrupt pin */
	io_conf.intr_type = GPIO_INTR_NEGEDGE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = (1ULL << MONITOR_INTR_GPIO_NUMBER);
	io_conf.pull_up_en = 1;
	gpio_config(&io_conf);
	gpio_isr_handler_add(MONITOR_INTR_GPIO_NUMBER, on_interrupt, 0);
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
	gpio_set_level(MONITOR_ENABLE_GPIO_NUMBER, enable);
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
	adc = adc1_create();
	adc_enable(adc, true);
	adc_calibrate(adc);
	adc1_channel_init(6, 0/*dB*/);

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
