#include "libmcu/system.h"
#include "cli/cli.h"

int main(void)
{
	struct cli cli;

	system_init();

	cli_init(&cli, cli_io_create(), cli_commands, cli_commands_len);
	cli_run(&cli);

	/* never reach down here unless cli gets terminated by exit command */
	while (1) {
	}
}
