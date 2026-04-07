/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "pinmap.h"

#include <errno.h>

int pinmap_init(struct pinmap_periph *periph)
{
	if (periph == NULL) {
		return -EINVAL;
	}

	return 0;
}
