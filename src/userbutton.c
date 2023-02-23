/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "userbutton.h"
#include "libmcu/board.h"
#include "libmcu/logging.h"

static const void *user_btn_handle;
static button_handler_t active_handler;
static void *userctx;

static void on_button_event(enum button_event event,
		const struct button_data *info, void *ctx)
{
	unused(ctx);

	switch (event) {
	case BUTTON_EVT_CLICK:
		debug("click %u %u", info->time_released, info->click);
		break;
	case BUTTON_EVT_PRESSED:
		debug("pressed %u", info->time_pressed);
		break;
	case BUTTON_EVT_RELEASED:
		debug("released %u", info->time_released);
		break;
	case BUTTON_EVT_HOLDING:
		debug("holding %u", info->time_repeat);
		break;
	default:
		break;
	}

	if (active_handler) {
		(*active_handler)(event, info, userctx);
	}
}

bool userbutton_process(void)
{
	if (button_step() == BUTTON_NO_ACTIVITY) {
		return false;
	}

	return true;
}

void userbutton_set_handler(button_handler_t handler, void *ctx)
{
	active_handler = handler;
	userctx = ctx;
}

void userbutton_unset_handler(button_handler_t handler)
{
	if (active_handler == handler) {
		active_handler = NULL;
		userctx = NULL;
	}
}

void userbutton_init(const userbutton_read_state_func_t func)
{
	button_init(board_get_time_since_boot_ms);
	user_btn_handle = button_register(func, on_button_event, 0);
}
