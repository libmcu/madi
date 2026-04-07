/*
 * SPDX-FileCopyrightText: 2023 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LOGGING_H
#define LOGGING_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "libmcu/logging.h"

void logging_stdout_backend_init(void);

#if defined(__cplusplus)
}
#endif

#endif /* LOGGING_H */
