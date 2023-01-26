/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_ADC_H
#define FPL_ADC_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "padc/adc.h"

struct adc *adc_create(void);
int adc_channel_init(int channel);

#if defined(__cplusplus)
}
#endif

#endif
