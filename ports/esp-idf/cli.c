/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/cli_overrides.h"

#if defined(esp32s3)
#include "usb_serial_jtag.h"
#else
#include "uart0.h"
#endif

struct cli_io const *cli_io_create(void)
{
	static bool initialized;

	if (!initialized) {
#if defined(esp32s3)
		serial_jtag_init();
#else
		uart0_init(115200);
#endif
		initialized = true;
	}

	static const struct cli_io io = {
#if defined(esp32s3)
		.read = serial_jtag_read,
		.write = serial_jtag_write_async,
#else
		.read = uart0_read,
		.write = uart0_write_async,
#endif
	};

	return &io;
}
