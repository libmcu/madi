/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "libmcu/assert.h"
#include "nrf_pwr_mgmt.h"
#include "app_timer.h"

void board_init(void)
{
	int rc = nrf_pwr_mgmt_init();
	assert(rc == NRF_SUCCESS);

	rc = app_timer_init();
	assert(rc == NRF_SUCCESS);
	app_timer_resume();
}

void board_reboot(void)
{
	NVIC_SystemReset();
}

/* FIXME: This is not monotonic implementation. will be wrapped around with RTC
 * counter overflow. */
unsigned long board_get_time_since_boot_ms(void)
{
	uint32_t cnt = app_timer_cnt_get();
	unsigned long ms = cnt * ((APP_TIMER_CONFIG_RTC_FREQUENCY + 1) * 1000) /
			APP_TIMER_CLOCK_FREQ;
	return ms;
}
