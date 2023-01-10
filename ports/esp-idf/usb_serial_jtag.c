/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "driver/usb_serial_jtag.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUFSIZE				256

void serial_jtag_init(void)
{
	usb_serial_jtag_driver_config_t cfg = { BUFSIZE, BUFSIZE };

	if (usb_serial_jtag_driver_install(&cfg) != ESP_OK) {
	}
}

size_t serial_jtag_write_async(void const *data, size_t datasize)
{
	if (datasize == 0) {
		return 0;
	} else if (datasize == 1 && ((const char *)data)[0] == '\n') {
		usb_serial_jtag_write_bytes(" \b\n", 3, 0);
		return 1;
	}

	int written = usb_serial_jtag_write_bytes(data, datasize, 0);
	return written > 0 ? (size_t)written : 0;
}

size_t serial_jtag_read(void *buf, size_t bufsize)
{
	int len = usb_serial_jtag_read_bytes(buf, bufsize,
			(TickType_t)portMAX_DELAY);
	return len > 0 ? len : 0;
}
