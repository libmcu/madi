/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/cli_overrides.h"

#if !defined(CONFIG_CONSOLE_SUBSYS) && !defined(CONFIG_CONSOLE_GETCHAR)
#error CONFIG_CONSOLE_SUBSYS and CONFIG_CONSOLE_GETCHAR should be set
#endif

#include <zephyr/console/console.h>

static size_t cli_write(void const *data, size_t datasize)
{
	ssize_t len = console_write(NULL, data, datasize);

	return len > 0 ? (size_t)len : 0;
}

static size_t cli_read(void *buf, size_t bufsize)
{
	ssize_t len = console_read(NULL, buf, bufsize);

	return len > 0 ? (size_t)len : 0;
}

struct cli_io const *cli_io_create(void)
{
	static bool initialized;

	if (!initialized && console_init() == 0) {
		initialized = true;
	}

	static const struct cli_io io = {
		.read = cli_read,
		.write = cli_write,
	};

	return &io;
}
