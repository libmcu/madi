/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MADI_SELFTEST_H
#define MADI_SELFTEST_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

bool selftest(void);
bool selftest_button(uint8_t expected_clicks, uint32_t timeout_ms);

#if defined(__cplusplus)
}
#endif

#endif /* MADI_SELFTEST_H */
