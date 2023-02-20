/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USER_BUTTON_H
#define USER_BUTTON_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>

typedef int (*user_button_read_state_func_t)(void);

user_button_read_state_func_t user_button_gpio_init(
		void (*event_callback)(void));
void user_button_init(user_button_read_state_func_t func);
bool user_button_process(void);

#if defined(__cplusplus)
}
#endif

#endif /* USER_BUTTON_H */
