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

typedef void (*evtloop_handler_t)(void *ctx);

struct ao_event {
	evtloop_handler_t handler;
	void *ctx;
};

void evtloop_init(int priority, size_t stack_size_bytes);
int evtloop_post(void *event);
int evtloop_post_defer(void *event, uint32_t msec);

#if defined(__cplusplus)
}
#endif

#endif /* EVTLOOP_H */
