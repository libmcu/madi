#include "watchdog.h"
#include <assert.h>
#include "esp_task_wdt.h"

#if !defined(WDT_TIMEOUT_MSEC)
#define WDT_TIMEOUT_MSEC				5000
#endif

void watchdog_init(void)
{
	assert(WDT_TIMEOUT_MSEC >= 1000);
	ESP_ERROR_CHECK(esp_task_wdt_init(WDT_TIMEOUT_MSEC / 1000, true));
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
