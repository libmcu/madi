/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "libmcu/assert.h"
#include "nrf_pwr_mgmt.h"
#include "app_timer.h"

#include "nrf_sdh_freertos.h"
#include "FreeRTOS.h"
#include "task.h"

void board_init(void)
{
	int rc = nrf_pwr_mgmt_init();
	assert(rc == NRF_SUCCESS);
#if 0
	nrf_sdh_freertos_init(0, 0);
	vTaskStartScheduler();
#endif
}

void board_reboot(void)
{
	NVIC_SystemReset();
}
