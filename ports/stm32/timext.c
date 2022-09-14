/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/timext.h"
#include "stm32f1xx_hal.h"

#define time_after(goal, chasing)	((int)(goal)    - (int)(chasing) < 0)

void timeout_set(unsigned int *goal, unsigned int msec)
{
	*goal = HAL_GetTick() + msec;
}

bool timeout_is_expired(unsigned int goal)
{
	return time_after(goal, HAL_GetTick());
}

void sleep_ms(unsigned int msec)
{
	HAL_Delay(msec);
}
