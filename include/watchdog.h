/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_WATCHDOG_H
#define FPL_WATCHDOG_H

#if defined(__cplusplus)
extern "C" {
#endif

void watchdog_init(void);
int watchdog_alloc(void);
void watchdog_feed(int id);

#if defined(__cplusplus)
}
#endif

#endif /* FPL_WATCHDOG_H */
