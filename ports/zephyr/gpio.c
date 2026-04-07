/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/gpio.h"

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>

#include "pinmap.h"

#define LED0_NODE	DT_ALIAS(led0)

struct lm_gpio {
	struct lm_gpio_api api;
	const struct gpio_dt_spec *spec;
	gpio_flags_t flags;
	int value;
	lm_gpio_callback_t callback;
	void *callback_ctx;
};

static int enable_gpio(struct lm_gpio *self)
{
	self->flags = GPIO_OUTPUT_INACTIVE;
	return gpio_pin_configure_dt(self->spec, GPIO_OUTPUT_INACTIVE);
}

static int disable_gpio(struct lm_gpio *self)
{
	return gpio_pin_configure(self->spec->port, self->spec->pin,
				  GPIO_DISCONNECTED);
}

static int enable_interrupt(struct lm_gpio *self)
{
	(void)self;
	return -ENOTSUP;
}

static int disable_interrupt(struct lm_gpio *self)
{
	(void)self;
	return -ENOTSUP;
}

static int set_gpio(struct lm_gpio *self, int value)
{
	self->value = value;
	return gpio_pin_set_dt(self->spec, value);
}

static int get_gpio(struct lm_gpio *self)
{
	if ((self->flags & GPIO_OUTPUT) && !(self->flags & GPIO_INPUT)) {
		return self->value;
	}
	return gpio_pin_get_dt(self->spec);
}

static int register_callback(struct lm_gpio *self,
			     lm_gpio_callback_t callback, void *ctx)
{
	self->callback = callback;
	self->callback_ctx = ctx;
	return 0;
}

static const struct lm_gpio_api gpio_api = {
	.enable            = enable_gpio,
	.disable           = disable_gpio,
	.enable_interrupt  = enable_interrupt,
	.disable_interrupt = disable_interrupt,
	.set               = set_gpio,
	.get               = get_gpio,
	.register_callback = register_callback,
};

static const struct gpio_dt_spec led_spec = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static struct lm_gpio led_gpio;

struct lm_gpio *lm_gpio_create(uint16_t pin)
{
	if (pin != PINMAP_LED) {
		return NULL;
	}

	led_gpio.spec = &led_spec;
	led_gpio.api  = gpio_api;

	return &led_gpio;
}

void lm_gpio_delete(struct lm_gpio *self)
{
	(void)self;
}
