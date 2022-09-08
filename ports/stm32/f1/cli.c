#include "libmcu/cli.h"
#include "uart1.h"

struct cli_io const *cli_io_create(void)
{
	static bool initialized;

	if (!initialized) {
		uart1_init(115200);
	}

	static const struct cli_io io = {
		.read = uart1_read,
		.write = uart1_write,
	};

	return &io;
}
