/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_UTIL_H
#define FPL_UTIL_H

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(CONCAT)
#define CONCAT_INTERNAL(a, b)	a ## b
#define CONCAT(a, b)		CONCAT_INTERNAL(a, b)
#endif

#if defined(__cplusplus)
}
#endif

#endif /* FPL_UTIL_H */
