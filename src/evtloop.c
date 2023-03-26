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

static struct ao evtloop;

static void dispatch(struct ao * const ao, const struct ao_event * const event)
{
	(void)ao;

	if (event && event->handler) {
		(*event->handler)(event->ctx);
	}
}

int evtloop_post(void *event)
{
	return ao_post(&evtloop, (struct ao_event * const)event);
}

int evtloop_post_defer(void *event, uint32_t msec)
{
	return ao_post_defer(&evtloop, (struct ao_event * const)event, msec);
}

void evtloop_init(int priority, size_t stack_size_bytes)
{
	ao_timer_init();
	ao_create(&evtloop, stack_size_bytes, priority);
	ao_start(&evtloop, dispatch);
}
