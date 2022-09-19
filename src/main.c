/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "cli/cli.h"

int main(void)
{
	struct cli cli;

	board_init();

	cli_init(&cli, cli_io_create(), cli_commands, cli_commands_len);
	cli_run(&cli);

	/* never reach down here unless cli gets terminated by exit command */
	while (1) {
	}
}
