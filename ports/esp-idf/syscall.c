/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/syscall.h"
#include <stdio.h>

static syscall_writer_t syscall_write;

static int write_wrapper(void *cookie, const char *ptr, int len)
{
	(void)cookie;

	int rc = 0;

	if (syscall_write) {
		rc = (*syscall_write)((const void *)ptr, (size_t)len);
	}

	return rc;
}

void syscall_register_writer(syscall_writer_t writer)
{
	syscall_write = writer;

	static char stdout_buf[128];
	fclose(stdout);
	stdout = fwopen(NULL, write_wrapper);
	setvbuf(stdout, stdout_buf, _IOLBF, sizeof(stdout_buf));
}

void syscall_register_reader(syscall_reader_t reader)
{
	(void)reader;
}
