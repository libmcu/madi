/*
 * SPDX-FileCopyrightText: 2023 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_system.h"

#if defined(CONFIG_MBEDTLS_CUSTOM_MEM_ALLOC)
#include "mbedtls/platform.h"
#include "esp_heap_caps.h"

#define PSRAM_ALLOCATION_THRESHOLD	CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL

static void *tls_calloc(size_t n, size_t size)
{
	const size_t bytes = n * size;
	void *p = NULL;

	if (bytes >= PSRAM_ALLOCATION_THRESHOLD) {
		if ((p = heap_caps_calloc(1, bytes,
				MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT))) {
			return p;
		}
	}

	/* allocate from internal memory */
	if ((p = heap_caps_calloc(1, bytes,
			MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT))) {
		return p;
	}

	/* fallback to any 8-bit capable memory */
	return heap_caps_calloc(1, bytes, MALLOC_CAP_8BIT);
}

static void tls_free(void *p)
{
	heap_caps_free(p);
}
#endif /* CONFIG_MBEDTLS_CUSTOM_MEM_ALLOC */

extern int main(void);
extern void app_main(void);

static void esp_init(void)
{
	if (nvs_flash_init() != ESP_OK) {
		nvs_flash_erase();
		nvs_flash_init();
	}

	esp_event_loop_create_default();

#if defined(CONFIG_MBEDTLS_CUSTOM_MEM_ALLOC)
	mbedtls_platform_set_calloc_free(tls_calloc, tls_free);
#endif
}

void app_main(void)
{
	esp_init();
	main();
}
