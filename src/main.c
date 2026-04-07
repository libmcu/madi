/*
 * SPDX-FileCopyrightText: 2023 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/board.h"
#include "libmcu/timext.h"
#include "libmcu/gpio.h"

#include "logging.h"
#include "console_sync.h"
#include "pinmap.h"

int main(void)
{
	board_init(); /* should be called very first. */
	console_sync_init();

	logging_init(board_get_time_since_boot_ms);
	logging_stdout_backend_init();

	const board_reboot_reason_t reboot_reason = board_get_reboot_reason();
	info("[%s] %s %s", board_get_reboot_reason_string(reboot_reason),
			board_get_serial_number_string(),
			board_get_version_string());

	struct lm_gpio *led = lm_gpio_create(PINMAP_LED);
	lm_gpio_enable(led);

	while (1) {
		lm_gpio_set(led, lm_gpio_get(led) ^ 1);
		sleep_ms(500);
	}

	return 0;
}
