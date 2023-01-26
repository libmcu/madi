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

void user_button_init(void (*event_callback)(void));
void user_button_init_hw(void (*event_callback)(void));
int user_button_get_state(void);
bool user_button_process(void);

#if defined(__cplusplus)
}
#endif

#endif /* USER_BUTTON_H */
