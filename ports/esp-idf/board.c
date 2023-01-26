/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "libmcu/ao_timer.h"
#include "esp_timer.h"
#include "driver/gpio.h"

static void wake_ao_timer(void *arg)
{
	(void)arg;

	static TickType_t previous_tick_count;
	TickType_t current_tick_count = xTaskGetTickCount();
	TickType_t elapsed = current_tick_count - previous_tick_count;
	uint32_t elapsed_ms = elapsed * 1000 / configTICK_RATE_HZ;

	if (elapsed_ms) {
		ao_timer_step(elapsed_ms);
	}

	previous_tick_count = current_tick_count;
}

static void initialize_ao(void)
{
	ao_timer_reset();

	const esp_timer_create_args_t timer_args = {
		.callback = &wake_ao_timer,
	};
	esp_timer_handle_t ao_timer;
	esp_timer_create(&timer_args, &ao_timer);
	esp_timer_start_periodic(ao_timer, 50000);
}

void board_reboot(void)
{
	esp_restart();
}

void board_init(void)
{
	gpio_install_isr_service(0);

	initialize_ao();
}
