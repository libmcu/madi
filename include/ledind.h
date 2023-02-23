/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LEDIND_H
#define LEDIND_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

enum ledind_mode {
	LEDIND_STATIC,
	LEDIND_BLINK,
	LEDIND_FADE,
};

struct ledind_api {
	int (*set)(bool on);
	int (*toggle)(void);
	int (*enable)(bool enable);
	/* TODO: add PWM functionality */
};

const struct ledind_api *ledind_gpio_create(void);
void ledind_init(const struct ledind_api *api);
void ledind_enable(void);
void ledind_disable(void);

/**
 * @brief Process LED indication
 *
 * @return milliseconds to the next period
 */
uint32_t ledind_step(void);
void ledind_set(enum ledind_mode mode,
		uint32_t on_period_ms, uint32_t off_period_ms);
void ledind_on(void);
void ledind_off(void);
void ledind_toggle(void);

#if defined(__cplusplus)
}
#endif

#endif /* LEDIND_H */
