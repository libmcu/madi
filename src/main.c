/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "libmcu/logging.h"
#include "libmcu/cli.h"

int main(void)
{
	logging_init();
	board_init();

	info("[%s] %s %s", board_get_reboot_reason_string(),
			board_get_serial_number_string(),
			board_get_version_string());

	static char cli_buffer[CLI_CMD_MAXLEN];
	struct cli cli;
	DEFINE_CLI_CMD_LIST(cli_commands,
			help, exit, info, reboot, md, wifi, ble, mqtt);
	cli_init(&cli, cli_io_create(), cli_buffer, sizeof(cli_buffer));
	cli_register_cmdlist(&cli, cli_commands);
	cli_run(&cli);

	return 0;
}
