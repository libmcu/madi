/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "libmcu/assert.h"
#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_clock.h"

#include "nrf_sdh.h"
#include "nrf_sdh_freertos.h"
#include "FreeRTOS.h"
#include "task.h"

#define MAIN_TASK_STACK_SIZE		2048U
#define MAIN_TASK_PRIORITY		1U

static void start_scheduler(void)
{
	extern int main(void);
	xTaskCreate((void (*)(void *))main, "Main",
			MAIN_TASK_STACK_SIZE / sizeof(StackType_t), 0,
			MAIN_TASK_PRIORITY, 0);
	vTaskStartScheduler();
}

static void initialize_bsp(void)
{
	ret_code_t rc = nrf_drv_clock_init();
	assert(rc == NRF_SUCCESS);

	rc = nrf_pwr_mgmt_init();
	assert(rc == NRF_SUCCESS);

	rc = nrf_drv_ppi_init();
	assert(rc == NRF_SUCCESS);
	rc = nrf_drv_gpiote_init();
	assert(rc == NRF_SUCCESS);
}

static void initialize_ble(void)
{
	nrf_sdh_enable_request();
	nrf_sdh_freertos_init(0, 0);
}

const char *board_get_serial_number_string(void)
{
	static char sn[16+1];

	if (!sn[0]) {
		sprintf(sn, "%08lx%08lx",
			NRF_FICR->DEVICEADDR[0], NRF_FICR->DEVICEADDR[1]);
	}

	return sn;
}

void board_reboot(void)
{
	NVIC_SystemReset();
}

void board_init(void)
{
	static bool initialized;

	if (!initialized) {
		initialized = true;

		initialize_bsp();
		initialize_ble();
		start_scheduler();
		return; /* never reaches down here */
	}
}
