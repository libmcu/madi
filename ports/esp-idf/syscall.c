/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/syscall.h"

void syscall_register_writer(syscall_writer_t writer)
{
	(void)writer;
}

void syscall_register_reader(syscall_reader_t reader)
{
	(void)reader;
}
