/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "esp_system.h"
#include "driver/gpio.h"

void board_reboot(void)
{
	esp_restart();
}

void board_init(void)
{
	gpio_install_isr_service(0);
}
