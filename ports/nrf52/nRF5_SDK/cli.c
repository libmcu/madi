/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/cli_overrides.h"
#include "uart0.h"

struct cli_io const *cli_io_create(void)
{
	static bool initialized;

	if (!initialized) {
		uart0_init(115200);
		initialized = true;
	}

	static const struct cli_io io = {
		.read = uart0_read,
		.write = uart0_write,
	};

	return &io;
}
