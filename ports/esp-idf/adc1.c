/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "adc1.h"

#include <errno.h>

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "libmcu/metrics.h"

#define ADC_RESOLUTION		ADC_BITWIDTH_12

#define VOLTAGE_DIVIDER_RATIO	134 /* R1: 360K, R2: 56K */

#if defined(esp32s3) || defined(esp32c3)
#define cal_type		adc_cali_curve_fitting_config_t
#define cal_func		adc_cali_create_scheme_curve_fitting
#define cal_del			adc_cali_delete_scheme_curve_fitting
#else
#define cal_type		adc_cali_line_fitting_config_t
#define cal_func		adc_cali_create_scheme_line_fitting
#define cal_del			adc_cali_delete_scheme_line_fitting
#endif

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc1_cal_handle;

static int initialize_adc1(void)
{
	adc_oneshot_unit_init_cfg_t cfg = {
		.unit_id = ADC_UNIT_1,
	};

	if (adc_oneshot_new_unit(&cfg, &adc1_handle) == ESP_OK) {
		return 0;
	}

	metrics_increase(ADCError);
	return -EFAULT;
}

static int initialize_channel(int channel, int attenuation)
{
	adc_oneshot_chan_cfg_t cfg = {
		.bitwidth = ADC_RESOLUTION,
		.atten = attenuation,
	};

	if (adc_oneshot_config_channel(adc1_handle, channel, &cfg) == ESP_OK) {
		return 0;
	}

	metrics_increase(ADCError);
	return -EFAULT;
}

static int calibrate_internal(int attenuation)
{
	cal_type cfg = {
		.unit_id = ADC_UNIT_1,
		.atten = attenuation,
		.bitwidth = ADC_RESOLUTION,
	};

	if (cal_func(&cfg, &adc1_cal_handle)
			== ESP_OK) {
		return 0;
	}

	metrics_increase(ADCError);
	return -EFAULT;
}

static int convert_raw_to_millivolts(int raw_adc)
{
	int mv;

	if (adc_cali_raw_to_voltage(adc1_cal_handle, raw_adc, &mv) == ESP_OK) {
		return mv;
	}

	metrics_increase(ADCError);
	return 0;
}

static int get_raw_internal(int channel)
{
	int adc;

	if (adc_oneshot_read(adc1_handle, channel, &adc) == ESP_OK) {
		return adc;
	}

	metrics_increase(ADCError);
	return 0;
}

static int enable(struct adc *self, bool enable)
{
	(void)self;

	/* 100mV ~ 950mV  : No attenuation
	 * 100mV ~ 1250mV : 2.5dB
	 * 150mV ~ 1750mV : 6dB
	 * 150mV ~ 2450mV : 11dB */
	if (enable) {
		return initialize_adc1();
	}

	/* disable */
	adc_oneshot_del_unit(adc1_handle);
	cal_del(adc1_cal_handle);

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
	int mv = convert_raw_to_millivolts(raw);
	return mv * 1000 / VOLTAGE_DIVIDER_RATIO;
}

static int calibrate(struct adc *self)
{
	(void)self;
	return calibrate_internal(ADC_ATTEN_DB_11);
}

int adc1_channel_init(int channel, int attenuation)
{
	return initialize_channel(channel, attenuation);
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
