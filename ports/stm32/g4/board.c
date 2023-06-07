/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "libmcu/assert.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "stm32g4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#define MAIN_TASK_STACK_SIZE		3072U
#define MAIN_TASK_PRIORITY		1U

static void start_scheduler(void)
{
	extern int main(void);
	if (xTaskCreate((void (*)(void *))main, "Main",
			MAIN_TASK_STACK_SIZE / sizeof(StackType_t), 0,
			MAIN_TASK_PRIORITY, 0) != pdPASS) {
		assert(0);
	}
	vTaskStartScheduler();
}

static void initialize_bsp(void)
{
	extern void SystemClock_Config(void);
	HAL_Init();
	SystemClock_Config();
}

const char *board_get_serial_number_string(void)
{
	static char sn[16+1];

	if (strnlen(sn, sizeof(sn)) == 0) {
		sprintf(sn, "%08lx%08lx", HAL_GetDEVID(), HAL_GetREVID());
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
		start_scheduler();
		return; /* never reaches down here */
	}
}
