/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_ADC1_H
#define ESP_ADC1_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "padc/adc.h"

struct adc *adc1_create(void);
int adc1_channel_init(int channel, int attenuation);

#if defined(__cplusplus)
}
#endif

#endif
