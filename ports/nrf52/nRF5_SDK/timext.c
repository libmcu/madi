/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/timext.h"
#include "nrf_delay.h"

void sleep_ms(unsigned int msec)
{
	nrf_delay_ms(msec);
}
