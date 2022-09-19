/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_random.h"

const char *board_get_serial_number_string(void)
{
	static char sn[18];

	if (sn[0] == '\0') {
		uint8_t mac[6] = { 0, };
		esp_efuse_mac_get_default(mac);
		snprintf(sn, sizeof(sn), "%02x:%02x:%02x:%02x:%02x:%02x",
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}

	return sn;
}

const char *board_get_reboot_reason_string(void)
{
	switch (esp_reset_reason()) {
	case ESP_RST_POWERON:
		return "Power On";
	case ESP_RST_EXT:
		return "Power Pin";
	case ESP_RST_SW:
		return "Software";
	case ESP_RST_PANIC:
		return "Panic";
	case ESP_RST_INT_WDT:
		return "Interrupt Watchdog";
	case ESP_RST_TASK_WDT:
		return "Task Watchdog";
	case ESP_RST_WDT:
		return "Watchdog";
	case ESP_RST_DEEPSLEEP:
		return "Deep Sleep";
	case ESP_RST_BROWNOUT:
		return "Brownout";
	case ESP_RST_SDIO:
		return "SDIO";
	default:
		return "Unknown";
	}
}

unsigned int board_get_free_heap_bytes(void)
{
	return esp_get_free_heap_size();
}

unsigned int board_get_heap_watermark(void)
{
	return esp_get_minimum_free_heap_size();
}

void board_reboot(void)
{
	esp_restart();
}

void board_init(void)
{
}
