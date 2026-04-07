/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef CONSOLE_SYNC_H
#define CONSOLE_SYNC_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>

int console_sync_init(void);
int console_sync_deinit(void);
int console_sync_write(const void *data, size_t datasize);
int console_sync_read(void *buf, size_t bufsize);

#if defined(__cplusplus)
}
#endif

#endif /* CONSOLE_SYNC_H */
