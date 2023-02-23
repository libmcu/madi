/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USERBUTTON_H
#define USERBUTTON_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>
#include "libmcu/button.h"

typedef int (*userbutton_read_state_func_t)(void);

userbutton_read_state_func_t userbutton_gpio_init(
		void (*event_callback)(void));
void userbutton_init(userbutton_read_state_func_t func);
bool userbutton_process(void);

void userbutton_set_handler(button_handler_t handler, void *ctx);
void userbutton_unset_handler(button_handler_t handler);

#if defined(__cplusplus)
}
#endif

#endif /* USERBUTTON_H */
