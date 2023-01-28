/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include <stdbool.h>

#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#define MAIN_TASK_STACK_SIZE		2048U
#define MAIN_TASK_PRIORITY		1U

static void start_scheduler(void)
{
	extern int main(void);
	xTaskCreate(main, "Main",
			MAIN_TASK_STACK_SIZE / sizeof(StackType_t), 0,
			MAIN_TASK_PRIORITY, 0);
	vTaskStartScheduler();
}

static void initialize_bsp(void)
{
	extern void SystemClock_Config(void);
	HAL_Init();
	SystemClock_Config();
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
