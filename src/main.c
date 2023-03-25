/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>

#include "libmcu/board.h"
#include "libmcu/logging.h"
#include "libmcu/cli.h"
#include "libmcu/syscall.h"

#include "evtloop.h"
#include "ledind.h"

#define EVENTLOOP_STACK_SIZE_BYTES	4096U
#define CLI_MAX_HISTORY			10U

static size_t logging_stdout_writer(const void *data, size_t size)
{
	unused(size);
	static char buf[LOGGING_MESSAGE_MAXLEN];
	size_t len = logging_stringify(buf, sizeof(buf), data);

	buf[len++] = '\n';
	buf[len] = '\0';

	write(1, buf, len);

	return len;
}

static void logging_stdout_backend_init(void)
{
	syscall_register_writer(cli_io_create()->write);

	static struct logging_backend log_console = { 0, };
	log_console.write = logging_stdout_writer;
	logging_add_backend(&log_console);
}

static void shell_start(void)
{
	static char cli_buffer[CLI_CMD_MAXLEN * CLI_MAX_HISTORY];
	struct cli cli;

	DEFINE_CLI_CMD_LIST(cli_commands,
			help, exit, info, reboot, md, wifi, ble);

	cli_init(&cli, cli_io_create(), cli_buffer, sizeof(cli_buffer));
	cli_register_cmdlist(&cli, cli_commands);
	cli_run(&cli);
}

int main(void)
{
	board_init(); /* should be called very first. */
	evtloop_init(1, EVENTLOOP_STACK_SIZE_BYTES);

	logging_init(board_get_time_since_boot_ms);
	logging_stdout_backend_init();

	info("[%s] %s %s", board_get_reboot_reason_string(),
			board_get_serial_number_string(),
			board_get_version_string());

	ledind_init(ledind_gpio_create());
	ledind_enable();
	ledind_on();

	shell_start();

	return 0;
}
