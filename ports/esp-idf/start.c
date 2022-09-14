/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_system.h"

#if !defined(DEFAULT_TASK_PRIORITY)
#define DEFAULT_TASK_PRIORITY				1
#endif
#if !defined(DEFAULT_TASK_STACK_SIZE)
#define DEFAULT_TASK_STACK_SIZE				3072
#endif

extern int main(void);
extern void app_main(void);

static void esp_init(void)
{
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(nvs_flash_init());
}

static void app_wrapper(void *e)
{
	(void)e;
	main();
	vTaskDelete(NULL);
}

void app_main(void)
{
	esp_init();

	xTaskCreate(app_wrapper,
			"app",
			DEFAULT_TASK_STACK_SIZE,
			NULL,
			DEFAULT_TASK_PRIORITY,
			NULL);
}
