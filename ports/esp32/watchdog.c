#include "watchdog.h"
#include "esp_task_wdt.h"
#include "libmcu/assert.h"

#define DEFAULT_WDT_TIMEOUT_SEC				5

void watchdog_init(void)
{
	ESP_ERROR_CHECK(esp_task_wdt_init(DEFAULT_WDT_TIMEOUT_SEC, true));
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
