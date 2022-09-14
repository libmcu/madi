/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "watchdog.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/task_wdt/task_wdt.h>

#if !defined(WDT_TIMEOUT_MSEC)
#define WDT_TIMEOUT_MSEC				5000
#endif

#if DT_NODE_HAS_STATUS(DT_ALIAS(watchdog0), okay)
#define WDT_NODE DT_ALIAS(watchdog0)
#elif DT_HAS_COMPAT_STATUS_OKAY(st_stm32_window_watchdog)
#define WDT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(st_stm32_window_watchdog)
#elif DT_HAS_COMPAT_STATUS_OKAY(st_stm32_watchdog)
#define WDT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(st_stm32_watchdog)
#elif DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf_wdt)
#define WDT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(nordic_nrf_wdt)
#elif DT_HAS_COMPAT_STATUS_OKAY(espressif_esp32_watchdog)
#define WDT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(espressif_esp32_watchdog)
#elif DT_HAS_COMPAT_STATUS_OKAY(silabs_gecko_wdog)
#define WDT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(silabs_gecko_wdog)
#elif DT_HAS_COMPAT_STATUS_OKAY(nxp_kinetis_wdog32)
#define WDT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(nxp_kinetis_wdog32)
#elif DT_HAS_COMPAT_STATUS_OKAY(microchip_xec_watchdog)
#define WDT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(microchip_xec_watchdog)
#else
#define WDT_NODE DT_INVALID_NODE
#endif

void watchdog_init(void)
{
	struct device const * const hw_wdt_dev = DEVICE_DT_GET_OR_NULL(WDT_NODE);

	if (!device_is_ready(hw_wdt_dev)) {
		printk("Hardware watchdog not ready; ignoring it.\n");
	}

	int ret = task_wdt_init(hw_wdt_dev);
	if (ret != 0) {
		printk("task wdt init failure: %d\n", ret);
		return;
	}
}

int watchdog_alloc(void)
{
	return task_wdt_add(WDT_TIMEOUT_MSEC, 0, 0);
}

void watchdog_feed(int id)
{
	task_wdt_feed(id);
}
