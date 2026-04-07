/*
 * SPDX-FileCopyrightText: 2023 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "logging.h"
#include "console_sync.h"

static size_t write_stdout(const void *data, size_t size)
{
	static char buf[LOGGING_MESSAGE_MAXLEN];
	size_t len = logging_stringify(buf, sizeof(buf)-2, data);
	size_t total_written = 0;
	unused(size);

	buf[len++] = '\n';
	buf[len] = '\0';

	while (total_written < len) {
		int rc = console_sync_write(buf + total_written,
				len - total_written);
		if (rc <= 0) { /* Treat short write or error as failure */
			return 0;
		}
		total_written += (size_t)rc;
	}

	return total_written;
}

void logging_stdout_backend_init(void)
{
	static struct logging_backend log_stdout = {
		.write = write_stdout,
	};

	logging_add_backend(&log_stdout);
}
