/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/cli_overrides.h"
#include "pusb/usbd_cdc.h"

struct cli_io const *cli_io_create(void)
{
	static bool initialized;

	if (!initialized) {
		if (usbd_cdc_init() == 0) {
			initialized = true;
		}
	}

	static const struct cli_io io = {
		.read = usbd_cdc_read,
		.write = usbd_cdc_write,
	};

	return &io;
}
