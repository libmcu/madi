/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "battery.h"
#include "adc1.h"
#include "driver/gpio.h"
#include "esp_attr.h"

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

static int enable_monitor(bool enable)
{
	adc_enable(adc, enable);

	if (enable) {
		adc_calibrate(adc);
		adc1_channel_init(6, 3/*11dB*/);
	}

	gpio_set_level(MONITOR_ENABLE_GPIO_NUMBER, enable);

	return 0;
}

static int get_level_adc(void)
{
	return adc_get_raw(adc, ADC_CHANNEL);
}

static int adc_to_millivolts(int raw)
{
	return adc_raw_to_millivolts(adc, raw);
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

	if ((adc = adc1_create()) == NULL) {
		return NULL;
	}

	return &mon;
}
