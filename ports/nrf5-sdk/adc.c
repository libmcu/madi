/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "adc.h"

#include <errno.h>
#include <stdbool.h>

#include "nrfx_saadc.h"

#include "libmcu/metrics.h"

#define ADC_RESOLUTION		NRF_SAADC_RESOLUTION_12BIT
#define ADC_PRIORITY		6

#define VOLTAGE_DIVIDER_RATIO	134 /* R1: 360K, R2: 56K */

static nrf_saadc_value_t raw_buffer[1]; /* nRF52840 supports up to 8 channels */

static int initialize_adc(void)
{
	if (nrfx_saadc_init(ADC_PRIORITY) != NRFX_SUCCESS) {
		metrics_increase(ADCError);
		return -EFAULT;
	}

	return 0;
}

static int initialize_adc_mode(int channel)
{
	channel = 1 << channel;
	return (int)nrfx_saadc_simple_mode_set((uint32_t)channel,
			ADC_RESOLUTION, NRF_SAADC_OVERSAMPLE_DISABLED, NULL);
}

static int initialize_channel(int channel)
{
	nrfx_saadc_channel_t cfg = {
		.channel_config = {
			.resistor_p = NRF_SAADC_RESISTOR_DISABLED,
			.resistor_n = NRF_SAADC_RESISTOR_DISABLED,
			.gain = NRF_SAADC_GAIN1, /* ~ 600mA */
			.reference = NRF_SAADC_REFERENCE_INTERNAL, /* 0.6V */
			.acq_time = NRF_SAADC_ACQTIME_15US, /* 192kOhm */
			.mode = NRF_SAADC_MODE_SINGLE_ENDED,
			.burst = NRF_SAADC_BURST_DISABLED,
			.pin_p = (nrf_saadc_input_t)(channel + NRF_SAADC_INPUT_AIN0),
			.pin_n = NRF_SAADC_INPUT_DISABLED,
		},
		.pin_p = (nrf_saadc_input_t)(channel + NRF_SAADC_INPUT_AIN0),
		.pin_n = NRF_SAADC_INPUT_DISABLED,
		.channel_index = (uint8_t)channel,
	};

	if (nrfx_saadc_channels_config(&cfg, 1) != NRFX_SUCCESS) {
		metrics_increase(ADCError);
		return -EFAULT;
	}
	if (initialize_adc_mode(channel) != NRFX_SUCCESS) {
		metrics_increase(ADCError);
		return -EFAULT;
	}
	if (nrfx_saadc_buffer_set(raw_buffer, sizeof(raw_buffer) /
				sizeof(raw_buffer[0])) != NRFX_SUCCESS) {
		metrics_increase(ADCError);
		return -ENOSPC;
	}

	return 0;
}

static int get_raw_internal(int channel)
{
	(void)channel;

	if (nrfx_saadc_mode_trigger() != NRFX_SUCCESS) {
		metrics_increase(ADCError);
		return 0;
	}

	return (int)raw_buffer[0];
}

static int enable(struct adc *self, bool enable)
{
	(void)self;

	if (enable) {
		return initialize_adc();
	}

	/* disable */
	nrfx_saadc_uninit();

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
	int mv = raw * (600/*=ref0.6V*1000mV*/) / 1/*gain*/ / (4096-1)/*12bit*/;
	return mv * 1000 / VOLTAGE_DIVIDER_RATIO;
}

static int calibrate(struct adc *self)
{
	(void)self;

	if (nrfx_saadc_offset_calibrate(NULL) != NRFX_SUCCESS) {
		metrics_increase(ADCError);
		return -EFAULT;
	}

	return 0;
}

int adc_channel_init(int channel)
{
	return initialize_channel(channel);
}

struct adc *adc_create(void)
{
	static struct adc_api api = {
		.get_raw_to_millivolts = get_raw_to_millivolts,
		.get_raw = get_raw,
		.enable = enable,
		.calibrate = calibrate,
	};

	return (struct adc *)&api;
}
