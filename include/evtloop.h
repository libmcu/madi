/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef EVTLOOP_H
#define EVTLOOP_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#if !defined(EVTLOOP_MAX_EVENT)
#define EVTLOOP_MAX_EVENT		10
#endif

typedef void (*evtloop_handler_t)(void *ctx);

void evtloop_init(int priority, size_t stack_size_bytes);
int evtloop_post(evtloop_handler_t handler, void *arg);
int evtloop_post_defer(evtloop_handler_t handler, void *arg, uint32_t msec);

#if defined(__cplusplus)
}
#endif

#endif /* EVTLOOP_H */
