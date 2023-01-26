/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "adc1.h"

#include <errno.h>

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "libmcu/metrics.h"

#define ADC_RESOLUTION			ADC_WIDTH_BIT_12

static esp_adc_cal_characteristics_t adc1_cal;

static int initialize_channel(int channel, int attenuation)
{
	if (adc1_config_channel_atten(channel, attenuation) != ESP_OK) {
		metrics_increase(ADCError);
		return -EFAULT;
	}

	return 0;
}

static int initialize_adc1(void)
{
	if (adc1_config_width(ADC_RESOLUTION) != ESP_OK) {
		metrics_increase(ADCError);
		return -EFAULT;
	}

	return 0;
}

static int calibrate_internal(int attenuation)
{
	if (esp_adc_cal_characterize(ADC_UNIT_1, attenuation, ADC_RESOLUTION,
			0, &adc1_cal) == ESP_ADC_CAL_VAL_NOT_SUPPORTED) {
		metrics_increase(ADCError);
		return -EFAULT;
	}

	return 0;
}

static int get_raw_internal(int channel)
{
	return adc1_get_raw(channel);
}

static int convert_raw_to_millivolts(int raw_adc)
{
	return esp_adc_cal_raw_to_voltage(raw_adc, &adc1_cal);
}

static int enable(struct adc *self, bool enable)
{
	(void)self;

	/* 100mV ~ 950mV  : No attenuation
	 * 100mV ~ 1250mV : 2.5dB
	 * 150mV ~ 1750mV : 6dB
	 * 150mV ~ 2450mV : 11dB */
	if (enable) {
		initialize_adc1();
		return 0;
	}

	/* TODO: disable */

	return 0;
}

static int get_raw(struct adc *self, int channel)
{
	(void)self;
	return get_raw_internal(channel);
}

static int get_raw_to_millivolts(struct adc *self, int raw)
{
	(void)self;
	return convert_raw_to_millivolts(raw);
}

static int calibrate(struct adc *self)
{
	(void)self;
	return calibrate_internal(ADC_ATTEN_DB_0);
}

int adc1_channel_init(int channel, int attenuation)
{
	initialize_channel(channel, attenuation);
}

struct adc *adc1_create(void)
{
	static struct adc_api api = {
		.get_raw_to_millivolts = get_raw_to_millivolts,
		.get_raw = get_raw,
		.enable = enable,
		.calibrate = calibrate,
	};

	return (struct adc *)&api;
}
