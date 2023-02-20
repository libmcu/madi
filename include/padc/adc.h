/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBMCU_ADC_H
#define LIBMCU_ADC_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>

struct adc;
struct adc_api {
	int (*enable)(struct adc *self, bool enable);
	int (*get_raw)(struct adc *self, int channel);
	int (*get_raw_to_millivolts)(struct adc *self, int raw);
	int (*calibrate)(struct adc *self);
};

#include "padc/adc_interface.h"

#if defined(__cplusplus)
}
#endif

#endif /* LIBMCU_ADC_H */
