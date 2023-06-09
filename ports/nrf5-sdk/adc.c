/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/port/adc.h"
#include "libmcu/metrics.h"

#include <errno.h>
#include <string.h>

#include "nrfx_saadc.h"

#define ADC_RESOLUTION		NRF_SAADC_RESOLUTION_12BIT
#define ADC_PRIORITY		6

#define VOLTAGE_DIVIDER_RATIO	134 /* R1: 360K, R2: 56K */

struct adc {
	/* nRF52840 supports up to 8 channels */
	nrf_saadc_value_t raw_buffer[1];
	uint8_t unit;
	bool activated;
};

static nrf_saadc_input_t channel_to_nrf(adc_channel_t channel)
{
	return (nrf_saadc_input_t)(channel? __builtin_ctz(channel) + 2 : 1);
}

static int initialize_adc_mode(int channel)
{
	return (int)nrfx_saadc_simple_mode_set((uint32_t)(channel << 1),
			ADC_RESOLUTION, NRF_SAADC_OVERSAMPLE_DISABLED, NULL);
}

int adc_port_convert_to_millivolts(struct adc *self, int value)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	int mv = value * (600/*=ref0.6V*1000mV*/) / 1/*gain*/ / (4096-1)/*12bit*/;
	return mv * 1000 / VOLTAGE_DIVIDER_RATIO;
}

int adc_port_read(struct adc *self, adc_channel_t channel)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	if (nrfx_saadc_mode_trigger() != NRFX_SUCCESS) {
		metrics_increase(ADCErrorRead);
		return -ENOMSG;
	}

	(void)channel;
	return (int)self->raw_buffer[0];
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

	nrf_saadc_input_t ch = channel_to_nrf(channel);
	nrfx_saadc_channel_t cfg = {
		.channel_config = {
			.resistor_p = NRF_SAADC_RESISTOR_DISABLED,
			.resistor_n = NRF_SAADC_RESISTOR_DISABLED,
			.gain = NRF_SAADC_GAIN1, /* ~ 600mA */
			.reference = NRF_SAADC_REFERENCE_INTERNAL, /* 0.6V */
			.acq_time = NRF_SAADC_ACQTIME_15US, /* 192kOhm */
			.mode = NRF_SAADC_MODE_SINGLE_ENDED,
			.burst = NRF_SAADC_BURST_DISABLED,
			.pin_p = ch,
			.pin_n = NRF_SAADC_INPUT_DISABLED,
		},
		.pin_p = ch,
		.pin_n = NRF_SAADC_INPUT_DISABLED,
		.channel_index = (uint8_t)(ch-1),
	};

	if (nrfx_saadc_channels_config(&cfg, 1) != NRFX_SUCCESS) {
		metrics_increase(ADCErrorParam);
		return -EINVAL;
	}
	if (initialize_adc_mode(channel) != NRFX_SUCCESS) {
		metrics_increase(ADCFault);
		return -EFAULT;
	}
	if (nrfx_saadc_buffer_set(self->raw_buffer, sizeof(self->raw_buffer) /
				sizeof(self->raw_buffer[0])) != NRFX_SUCCESS) {
		metrics_increase(ADCFault);
		return -ENOSPC;
	}

	return 0;
}

int adc_port_calibrate(struct adc *self)
{
	if (!self || !self->activated) {
		metrics_increase(ADCErrorPipe);
		return -EPIPE;
	}

	if (nrfx_saadc_offset_calibrate(NULL) != NRFX_SUCCESS) {
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

	if (nrfx_saadc_init(ADC_PRIORITY) != NRFX_SUCCESS) {
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

	nrfx_saadc_uninit();

	self->activated = false;
	return 0;
}

struct adc *adc_port_create(uint8_t adc_num)
{
	static struct adc adc;

	if (adc_num != 0 || adc.activated) {
		return NULL;
	}

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
