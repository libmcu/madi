/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lwip/apps/fs.h"

int fs_open_custom(struct fs_file *file, const char *name)
{
	file->flags |= FS_FILE_FLAGS_HEADER_INCLUDED;
	(void)name;
	//return ERR_VAL;
	return ERR_OK;
}

void fs_close_custom(struct fs_file *file)
{
	(void)file;
}

#if LWIP_HTTPD_DYNAMIC_FILE_READ
int fs_read_custom(struct fs_file *file, char *buffer, int count)
int fs_read_async_custom(struct fs_file *file, char *buffer, int count, fs_wait_cb callback_fn, void *callback_arg)
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
