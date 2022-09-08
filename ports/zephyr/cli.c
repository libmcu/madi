#include <stdio.h>
#include "libmcu/cli.h"

static size_t cli_write(void const *data, size_t datasize)
{
	int len = fwrite(data, datasize, 1, stdout);
	return len > 0 ? (size_t)len : 0;
}

#if defined(esp32)
#include "esp32/rom/uart.h"
static size_t cli_read(void *buf, size_t bufsize)
{
	char *p = (char *)buf;
	size_t len = 0;

	while (len < bufsize) {
		if (uart_rx_one_char(&p[len]) == FAIL) {
			break;
		}
		len += 1;
	}

	return len;
}
#elif defined(nrf52832_mdk) || defined(stm32_min_dev_blue)
static size_t cli_read(void *buf, size_t bufsize) { return 0; }
#else
#error unsupported device
#endif

struct cli_io const *cli_io_create(void)
{
	static const struct cli_io io = {
		.read = cli_read,
		.write = cli_write,
	};

	return &io;
}
