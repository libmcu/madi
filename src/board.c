/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/board.h"
#include <stdio.h>
#include "version.h"

const char *board_get_version_string(void)
{
	static char version[16];

	if (version[0] == '\0') {
		snprintf(version, sizeof(version), "%d.%d.%d",
				GET_VERSION_MAJOR(APP_VERSION),
				GET_VERSION_MINOR(APP_VERSION),
				GET_VERSION_PATCH(APP_VERSION));
	}

	return version;
}
