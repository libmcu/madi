/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "uart0.h"
#include "app_uart.h"
#include "nrf.h"
#include "nrf_uarte.h"
#include "bsp.h"

#define UART_TX_BUF_SIZE	128
#define UART_RX_BUF_SIZE	128

#define IRQ_PRIO		APP_IRQ_PRIORITY_LOWEST

#define DEFAULT_BAUDRATE	NRF_UARTE_BAUDRATE_115200

static uint8_t rx_buf[UART_RX_BUF_SIZE];
static uint8_t tx_buf[UART_TX_BUF_SIZE];

static void on_error(app_uart_evt_t * p_event)
{
	if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR) {
	} else if (p_event->evt_type == APP_UART_FIFO_ERROR) {
	}
}

static uint32_t convert_baud(uint32_t baudrate)
{
	switch (baudrate) {
	case 1200:    return NRF_UARTE_BAUDRATE_115200;
	case 2400:    return NRF_UARTE_BAUDRATE_115200;
	case 4800:    return NRF_UARTE_BAUDRATE_115200;
	case 9600:    return NRF_UARTE_BAUDRATE_115200;
	case 14400:   return NRF_UARTE_BAUDRATE_115200;
	case 19200:   return NRF_UARTE_BAUDRATE_115200;
	case 28800:   return NRF_UARTE_BAUDRATE_115200;
	case 31250:   return NRF_UARTE_BAUDRATE_115200;
	case 38400:   return NRF_UARTE_BAUDRATE_115200;
	case 56000:   return NRF_UARTE_BAUDRATE_115200;
	case 57600:   return NRF_UARTE_BAUDRATE_115200;
	case 76800:   return NRF_UARTE_BAUDRATE_115200;
	case 115200:  return NRF_UARTE_BAUDRATE_115200;
	case 230400:  return NRF_UARTE_BAUDRATE_115200;
	case 250000:  return NRF_UARTE_BAUDRATE_115200;
	case 460800:  return NRF_UARTE_BAUDRATE_115200;
	case 921600:  return NRF_UARTE_BAUDRATE_115200;
	case 1000000: return NRF_UARTE_BAUDRATE_115200;
	default:      return DEFAULT_BAUDRATE;
	}
}

static int initialize_uart0(uint32_t baudrate)
{
	const app_uart_comm_params_t cfg = {
		RX_PIN_NUMBER,
		TX_PIN_NUMBER,
		RTS_PIN_NUMBER,
		CTS_PIN_NUMBER,
		APP_UART_FLOW_CONTROL_DISABLED,
		false,
		baudrate,
	};

	app_uart_buffers_t buffers = {
		.rx_buf = rx_buf,
		.rx_buf_size = sizeof(rx_buf),
		.tx_buf = tx_buf,
		.tx_buf_size = sizeof(tx_buf),
	};

        if (app_uart_init(&cfg, &buffers, on_error, IRQ_PRIO) != NRF_SUCCESS) {
		return -1;
	}

	return 0;
}

size_t uart0_write(const void *data, size_t datasize)
{
	size_t cnt = 0;

	for (size_t i = 0; i < datasize; i++) {
		while (app_uart_put(((const uint8_t *)data)[i]) != NRF_SUCCESS) {
		}
		cnt++;
	}

	return cnt;
}

size_t uart0_read(void *buf, size_t bufsize)
{
	size_t i = 0;

	while (app_uart_get(&((uint8_t *)buf)[i]) == NRF_SUCCESS && i < bufsize) {
		i++;
	}

	return i;
}

void uart0_init(uint32_t baudrate)
{
	initialize_uart0(convert_baud(baudrate));
}
