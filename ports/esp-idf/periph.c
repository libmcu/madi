/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "periph.h"

#include <errno.h>

#include "libmcu/compiler.h"

static int enable_ui_periph(struct pinmap_periph *periph)
{
	unused(periph);
	return -ENOTSUP;
}

static int disable_ui_periph(struct pinmap_periph *periph)
{
	unused(periph);
	return -ENOTSUP;
}

int periph_enable_specific(struct pinmap_periph *pinmap, periph_t periph)
{
	if (pinmap == NULL) {
		return -ENODEV;
	}

	if (periph != PERIPH_UI) {
		return -EINVAL;
	}

	return enable_ui_periph(pinmap);
}

int periph_disable_specific(struct pinmap_periph *pinmap, periph_t periph)
{
	if (pinmap == NULL) {
		return -ENODEV;
	}

	if (periph != PERIPH_UI) {
		return -EINVAL;
	}

	return disable_ui_periph(pinmap);
}

int periph_reset_specific(struct pinmap_periph *pinmap, periph_t periph)
{
	if (pinmap == NULL) {
		return -ENODEV;
	}

	if (periph != PERIPH_UI) {
		return -EINVAL;
	}

	int err = disable_ui_periph(pinmap);
	err |= enable_ui_periph(pinmap);

	return err;
}

int periph_enable(struct pinmap_periph *periph)
{
	if (periph == NULL) {
		return -ENODEV;
	}

	return enable_ui_periph(periph);
}

int periph_disable(struct pinmap_periph *periph)
{
	if (periph == NULL) {
		return -ENODEV;
	}

	return disable_ui_periph(periph);
}

int periph_init(struct pinmap_periph *periph)
{
	unused(periph);
	return 0;
}
