/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "evtloop.h"

#include <errno.h>

#include "libmcu/ao.h"
#include "libmcu/ao_timer.h"
#include "libmcu/ao_overrides.h"
#include "libmcu/assert.h"

struct ao_event {
	evtloop_handler_t handler;
	void *ctx;
};

static struct ao evtloop;
static struct ao_event event_pool[EVTLOOP_MAX_EVENT];

static struct ao_event *alloc_slot(void)
{
	for (int i = 0; i < EVTLOOP_MAX_EVENT; i++) {
		struct ao_event *p = &event_pool[i];
		if (p->handler == 0 && p->ctx == 0) {
			return p;
		}
	}
	return NULL;
}

static void free_slot(const struct ao_event *evt)
{
	for (int i = 0; i < EVTLOOP_MAX_EVENT; i++) {
		struct ao_event *p = &event_pool[i];
		if (p == evt) {
			p->handler = 0;
			p->ctx = 0;
			return;
		}
	}
}

static struct ao_event *alloc_event(evtloop_handler_t handler, void *arg)
{
	struct ao_event *evt;

	/* NOTE: using ao lock would cause performance degrade, but simplest
	 * way to not implement new one. */
	ao_lock(0);
	evt = alloc_slot();
	ao_unlock(0);

	if (!evt) {
		return NULL;
	}

	evt->handler = handler;
	evt->ctx = arg;

	return evt;
}

static void dispatch(struct ao * const ao, const struct ao_event * const event)
{
	(void)ao;

	if (event && event->handler) {
		(*event->handler)(event->ctx);
	}

	free_slot(event);
}

int evtloop_post(evtloop_handler_t handler, void *arg)
{
	assert(!(handler == NULL && arg == NULL));
	return ao_post(&evtloop, alloc_event(handler, arg));
}

int evtloop_post_defer(evtloop_handler_t handler, void *arg, uint32_t msec)
{
	assert(!(handler == NULL && arg == NULL));
	return ao_post_defer(&evtloop, alloc_event(handler, arg), msec);
}

void evtloop_init(int priority, size_t stack_size_bytes)
{
	ao_timer_init();
	ao_create(&evtloop, stack_size_bytes, priority);
	ao_start(&evtloop, dispatch);
}
