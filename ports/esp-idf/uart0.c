#include "driver/uart.h"

#include "libmcu/cli.h"

#define RXBUF_SIZE			256

static void initialize_uart0(uint32_t baudrate)
{
	uart_config_t uart_config = {
		.baud_rate = baudrate,
		.data_bits = UART_DATA_8_BITS,
		.parity    = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	};

	uart_driver_install(UART_NUM_0, RXBUF_SIZE, 0, 0, NULL, 0);
	uart_param_config(UART_NUM_0, &uart_config);
}

static size_t uart0_write(void const *data, size_t datasize)
{
	int written = uart_write_bytes(UART_NUM_0, data, datasize);

	return written > 0 ? (size_t)written : 0;
}

static size_t uart0_read(void *buf, size_t bufsize)
{
	int len = uart_read_bytes(UART_NUM_0, buf, bufsize, 0);

	return len > 0 ? (size_t)len : 0;
}

struct cli_io const *cli_io_create(void)
{
	initialize_uart0(115200);

	static const struct cli_io io = {
		.read = uart0_read,
		.write = uart0_write,
	};

	return &io;
}
