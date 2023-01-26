/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "user_button.h"
#include "libmcu/button.h"
#include "libmcu/board.h"
#include "libmcu/logging.h"

static const void *user_btn_handle;

static void on_pressed(const struct button_data *btn, void *context)
{
	unused(btn);
	unused(context);
	info("pressed");
}
static void on_released(const struct button_data *btn, void *context)
{
	unused(btn);
	unused(context);
	info("released");
}

bool user_button_process(void)
{
	button_poll(NULL);

	if (button_is_pressed(user_btn_handle)) {
		return true;
	}

	return false;
}

void user_button_init(void (*on_event_callback)(void))
{
	user_button_init_hw(on_event_callback);

	button_init(board_get_time_since_boot_ms);

	static struct button_handlers handlers = {
		.pressed = on_pressed,
		.released = on_released,
	};
	user_btn_handle = button_register(&handlers, user_button_get_state);
}
