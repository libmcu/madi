/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "watchdog.h"

#if !defined(WDT_TIMEOUT_MSEC)
#define WDT_TIMEOUT_MSEC				5000
#endif

void watchdog_init(void)
{
}

int watchdog_alloc(void)
{
	return 0;
}

void watchdog_feed(int id)
{
	(void)id;
}
