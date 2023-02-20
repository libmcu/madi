/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LIBMCU_ADC_INTERFACE_H
#define LIBMCU_ADC_INTERFACE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "padc/adc.h"

static inline int adc_enable(struct adc *self, bool enable)
{
	return ((struct adc_api *)self)->enable(self, enable);
}

static inline int adc_calibrate(struct adc *self)
{
	return ((struct adc_api *)self)->calibrate(self);
}

static inline int adc_get_raw(struct adc *self, int channel)
{
	return ((struct adc_api *)self)->get_raw(self, channel);
}

static inline int adc_raw_to_millivolts(struct adc *self, int raw)
{
	return ((struct adc_api *)self)->get_raw_to_millivolts(self, raw);
}

#if defined(__cplusplus)
}
#endif

#endif /* LIBMCU_ADC_INTERFACE_H */
