/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ledind.h"

#define DEFAULT_NEXT_PERIOD_MS		1000U

static struct {
	const struct ledind_api *api;
	uint32_t on_period_ms;
	uint32_t off_period_ms;
	enum ledind_mode mode;
	bool is_on;
	bool initialized;
} m;

void ledind_on(void)
{
	m.api->set(true);
	m.is_on = true;
}

void ledind_off(void)
{
	m.api->set(false);
	m.is_on = false;
}

void ledind_toggle(void)
{
	m.api->toggle();
	m.is_on ^= 1;
}

void ledind_set(enum ledind_mode mode,
		uint32_t on_period_ms, uint32_t off_period_ms)
{
	m.mode = mode;
	m.on_period_ms = on_period_ms;
	m.off_period_ms = off_period_ms;
}

uint32_t ledind_step(void)
{
	uint32_t next_period_ms = DEFAULT_NEXT_PERIOD_MS;

	switch (m.mode) {
	case LEDIND_BLINK:
		if (m.is_on) {
			next_period_ms = m.off_period_ms;
		} else {
			next_period_ms = m.on_period_ms;
		}
		ledind_toggle();
		break;
	case LEDIND_FADE: /* TODO: Implement PWM functionality */
		break;
	case LEDIND_STATIC: /* fall through */
		if (m.is_on != !!m.on_period_ms) {
			ledind_toggle();
		}
	default:
		break;
	}

	return next_period_ms;
}

void ledind_enable(void)
{
	m.api->enable(true);
}

void ledind_disable(void)
{
	m.api->enable(false);
}

void ledind_init(const struct ledind_api *api)
{
	m.api = api;
	m.on_period_ms = DEFAULT_NEXT_PERIOD_MS;
	m.off_period_ms = DEFAULT_NEXT_PERIOD_MS;
}
