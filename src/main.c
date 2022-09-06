#include "libmcu/system.h"
#include "libmcu/cli.h"
#include "libmcu/timext.h"

#include "cli/cli_commands.h"

int main(void)
{
	struct cli cli;

	system_init();

	cli_init(&cli, cli_io_create(), cli_commands, cli_commands_len);

	while (1) {
		cli_step(&cli);
		sleep_ms(10);
	}
}
