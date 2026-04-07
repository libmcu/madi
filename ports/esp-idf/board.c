/*
 * SPDX-FileCopyrightText: 2023 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/board.h"
#include "libmcu/mgmt.h"

#include "mgmt/mgmt.h"

#define MGMT_BUF_COUNT	2U
#define MGMT_BUF_SIZE	(MGMT_MAX_MTU + MGMT_HDR_SIZE)

static uint8_t s_mgmt_buf[MGMT_BUF_COUNT][MGMT_BUF_SIZE];

static void initialize_system_management(void)
{
	mgmt_init(&(struct mgmt_config){
		.buf_pool  = s_mgmt_buf,
		.buf_size  = sizeof(s_mgmt_buf[0]),
		.buf_count = MGMT_BUF_COUNT,
	});
}

void board_init(void)
{
	initialize_system_management();
}
