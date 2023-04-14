/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/cli_overrides.h"

#if defined(esp32s3)
#include "usb_serial_jtag.h"
#else
#include "libmcu/uart.h"
static struct uart *uart_handle;

static int read_uart(void *buf, size_t bufsize)
{
	return uart_read(uart_handle, buf, bufsize);
}

static int write_uart(void const *data, size_t datasize)
{
	return uart_write(uart_handle, data, datasize);
}
#endif

struct cli_io const *cli_io_create(void)
{
	static bool initialized;

	if (!initialized) {
#if defined(esp32s3)
		serial_jtag_init();
#else
		uart_handle = uart_create(0);
		uart_enable(uart_handle, 115200);
#endif
		initialized = true;
	}

	static const struct cli_io io = {
#if defined(esp32s3)
		.read = serial_jtag_read,
		.write = serial_jtag_write_async,
#else
		.read = read_uart,
		.write = write_uart,
#endif
	};

	return &io;
}
