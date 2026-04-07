/*
 * SPDX-FileCopyrightText: 2023 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/gpio.h"
#include "libmcu/compiler.h"

#include <errno.h>

#include "esp_attr.h"

#include "driver/gpio.h"
#include "pinmap.h"

struct lm_gpio {
	struct lm_gpio_api api;

	uint16_t pin;
	lm_gpio_callback_t callback;
	void *callback_ctx;

	int (*boot)(struct lm_gpio *self);
};

static IRAM_ATTR void on_gpio_interrupt(void *arg)
{
	struct lm_gpio *gpio = (struct lm_gpio *)arg;

	if (gpio->callback) {
		(*gpio->callback)(gpio, gpio->callback_ctx);
	}
}

static int set_input(struct lm_gpio *self,
		const gpio_int_type_t intr, const gpio_pullup_t pullup)
{
	gpio_config_t cnf = {
		.intr_type = intr,
		.mode = GPIO_MODE_INPUT,
		.pin_bit_mask = (1ULL << self->pin),
		.pull_up_en = pullup,
	};

	esp_err_t err = gpio_config(&cnf);

	if (intr != GPIO_INTR_DISABLE) {
		err |= gpio_isr_handler_add(self->pin, on_gpio_interrupt, self);
	}

	return err;
}

static int set_input_int_anyedge(struct lm_gpio *self)
{
	return set_input(self, GPIO_INTR_ANYEDGE, GPIO_PULLUP_DISABLE);
}

static int set_input_int_falling(struct lm_gpio *self)
{
	return set_input(self, GPIO_INTR_NEGEDGE, GPIO_PULLUP_DISABLE);
}

static int set_input_int_falling_pullup(struct lm_gpio *self)
{
	return set_input(self, GPIO_INTR_NEGEDGE, GPIO_PULLUP_ENABLE);
}

static int set_output_pullup(struct lm_gpio *self)
{
	gpio_config_t cnf = {
		.intr_type = GPIO_INTR_DISABLE,
		.mode = GPIO_MODE_INPUT_OUTPUT,
		.pin_bit_mask = (1ULL << self->pin),
		.pull_up_en = true,
	};

	return gpio_config(&cnf);
}

static struct lm_gpio gpio_tbl[] = {
	{ .pin = PINMAP_LED, .boot = set_output_pullup, },
};

static struct lm_gpio *find_gpio_by_pin(uint16_t pin)
{
	size_t len = sizeof(gpio_tbl) / sizeof(gpio_tbl[0]);

	for (size_t i = 0; i < len; i++) {
		if (gpio_tbl[i].pin == pin) {
			return &gpio_tbl[i];
		}
	}

	return NULL;
}

static int enable_gpio(struct lm_gpio *self)
{
	if (self->boot) {
		return self->boot(self);
	}

	return -ERANGE;
}

static int disable_gpio(struct lm_gpio *self)
{
	unused(self);
	return 0;
}

static int enable_interrupt(struct lm_gpio *self)
{
	return gpio_intr_enable(self->pin);
}

static int disable_interrupt(struct lm_gpio *self)
{
	return gpio_intr_disable(self->pin);
}

static int set_gpio(struct lm_gpio *self, int value)
{
	return gpio_set_level(self->pin, (uint32_t)value);
}

static int get_gpio(struct lm_gpio *self)
{
	return gpio_get_level(self->pin);
}

static int register_callback(struct lm_gpio *self,
		lm_gpio_callback_t cb, void *cb_ctx)
{
	self->callback = cb;
	self->callback_ctx = cb_ctx;
	return 0;
}

struct lm_gpio *lm_gpio_create(uint16_t pin)
{
	struct lm_gpio *p = find_gpio_by_pin(pin);

	if (p) {
		p->api = (struct lm_gpio_api) {
			.enable = enable_gpio,
			.disable = disable_gpio,
			.enable_interrupt = enable_interrupt,
			.disable_interrupt = disable_interrupt,
			.set = set_gpio,
			.get = get_gpio,
			.register_callback = register_callback,
		};
	}

	return p;
}

void lm_gpio_delete(struct lm_gpio *self)
{
	unused(self);
}
