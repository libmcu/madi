/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board/pm.h"
#include "esp_system.h"
#include "esp_sleep.h"

int pm_board_enter(pm_mode_t mode)
{
	switch (mode) {
	case PM_SLEEP_DEEP:
		esp_deep_sleep_start();
		break;
	case PM_SLEEP:
		esp_light_sleep_start();
		break;
	default:
		esp_restart();
		break;
	}

	return 0;
}
