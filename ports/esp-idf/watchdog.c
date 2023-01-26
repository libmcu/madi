/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "watchdog.h"
#include <assert.h>
#include "esp_task_wdt.h"
#include "esp_idf_version.h"

#if !defined(WDT_TIMEOUT_MSEC)
#define WDT_TIMEOUT_MSEC				5000
#endif

void watchdog_init(void)
{
	assert(WDT_TIMEOUT_MSEC >= 1000);
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)
	ESP_ERROR_CHECK(esp_task_wdt_init(WDT_TIMEOUT_MSEC / 1000, true));
#else
	esp_task_wdt_config_t cfg = {
		.timeout_ms = WDT_TIMEOUT_MSEC,
		.idle_core_mask = (1 << portNUM_PROCESSORS) - 1, /* Bitmask of all cores */
		.trigger_panic = false,
	};

	ESP_ERROR_CHECK(esp_task_wdt_init(&cfg));
#endif
}

int watchdog_alloc(void)
{
	ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
	return 0;
}

void watchdog_feed(int id)
{
	(void)id;
	ESP_ERROR_CHECK(esp_task_wdt_reset());
}
