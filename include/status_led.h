/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_STATUS_LED_H
#define FPL_STATUS_LED_H

#if defined(__cplusplus)
extern "C" {
#endif

void status_led_init(void);
void status_led_on(void);
void status_led_off(void);
void status_led_toggle(void);

#if defined(__cplusplus)
}
#endif

#endif /* FPL_STATUS_LED_H */
