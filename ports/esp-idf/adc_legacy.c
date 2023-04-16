/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>
#include <string.h>

#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "libmcu/port/adc.h"
#include "libmcu/metrics.h"

#define ADC_RESOLUTION			ADC_WIDTH_BIT_12
#define ADC_ATTEN			ADC_ATTEN_DB_11

#define VOLTAGE_DIVIDER_RATIO		134 /* R1: 360K, R2: 56K */

struct adc {
	esp_adc_cal_characteristics_t cal_handle;
	uint8_t unit;
	bool activated;
};

static int channel_to_esp(adc_channel_t channel)
{
	return channel? __builtin_ctz(channel) + 1 : 0;
}

int adc_port_convert_to_millivolts(struct adc *self, int value)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	int mv = esp_adc_cal_raw_to_voltage(value, &self->cal_handle);
	return mv * 1000 / VOLTAGE_DIVIDER_RATIO;
}

int adc_port_read(struct adc *self, adc_channel_t channel)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	return adc1_get_raw(channel_to_esp(channel));
}

int adc_port_measure(struct adc *self)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	return 0;
}

int adc_port_channel_init(struct adc *self, adc_channel_t channel)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	/* 100mV ~ 950mV  : No attenuation
	 * 100mV ~ 1250mV : 2.5dB
	 * 150mV ~ 1750mV : 6dB
	 * 150mV ~ 2450mV : 11dB */
	if (adc1_config_channel_atten(channel_to_esp(channel), ADC_ATTEN)
			!= ESP_OK) {
		metrics_increase(ADCErrorParam);
		return -EINVAL;
	}

	return 0;
}

int adc_port_calibrate(struct adc *self)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	if (esp_adc_cal_characterize(self->unit, ADC_ATTEN, ADC_RESOLUTION, 0,
			&self->cal_handle) == ESP_ADC_CAL_VAL_NOT_SUPPORTED) {
		metrics_increase(ADCErrorCalibration);
		return -ESTALE;
	}

	return 0;
}

int adc_port_enable(struct adc *self)
{
	if (!self) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	} else if (self->activated) {
		metrics_increase(ADCErrorSequence);
		return -EALREADY;
	}

	if (adc1_config_width(ADC_RESOLUTION) != ESP_OK) {
		metrics_increase(ADCFault);
		return -EFAULT;
	}

	self->activated = true;
	return 0;
}

int adc_port_disable(struct adc *self)
{
	if (!self) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	} else if (!self->activated) {
		metrics_increase(ADCErrorSequence);
		return -EALREADY;
	}

	/* TODO: disable ADC */
	self->activated = false;
	return 0;
}

struct adc *adc_port_create(uint8_t adc_num)
{
	static struct adc adc;

	if (adc_num != 1 || adc.activated) {
		return NULL;
	}

	adc.unit = 0;

	return &adc;
}

int adc_port_delete(struct adc *self)
{
	if (!self) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	} else if (self->activated) {
		adc_port_disable(self);
	}

	memset(self, 0, sizeof(*self));
	return 0;
}
