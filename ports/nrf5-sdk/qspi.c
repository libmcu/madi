/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nrf_drv_qspi.h"
#include <errno.h>
#include "nrf_drv_gpiote.h"
#include "qspi.h"

static volatile bool qspi_xfer_done;

static void qspi_handler(nrf_drv_qspi_evt_t event, void *p_context) {
	(void)event;
	(void)p_context;

	qspi_xfer_done = true;
}

static void poll_xfer_done(void)
{
	while (!qspi_xfer_done) {
		/* wait for the job to be done */
	}

	qspi_xfer_done = false;
}

static int initialize_qspi(void)
{
	nrf_drv_qspi_config_t cfg = {
		.xip_offset = 0,
		.pins = {
			.sck_pin = NRF_GPIO_PIN_MAP(0, 8),
			.csn_pin = NRF_GPIO_PIN_MAP(0, 7),
			.io0_pin = NRF_GPIO_PIN_MAP(1, 9), /* MOSI */
			.io1_pin = NRF_GPIO_PIN_MAP(1, 8), /* MISO */
			.io2_pin = NRF_GPIO_PIN_MAP(0, 12),
			.io3_pin = NRF_GPIO_PIN_MAP(0, 6),
		},
		.prot_if = {
			.readoc = NRF_QSPI_READOC_READ4IO,
			.writeoc = NRF_QSPI_WRITEOC_PP4IO,
			.addrmode = NRF_QSPI_ADDRMODE_24BIT,
			.dpmconfig = false,
		},
		.phy_if = {
			.sck_delay = 1,
			.dpmen = false,
			.spi_mode = NRF_QSPI_MODE_0,
			.sck_freq = NRF_QSPI_FREQ_32MDIV2,
		},
		.irq_priority = 6,
	};

	if (nrf_drv_qspi_init(&cfg, qspi_handler, NULL) != NRF_SUCCESS) {
		return -EFAULT;
	}

	nrf_gpio_cfg(NRF_GPIO_PIN_MAP(0, 7),
			NRF_GPIO_PIN_DIR_OUTPUT,
			NRF_GPIO_PIN_INPUT_DISCONNECT,
			NRF_GPIO_PIN_PULLUP,
			NRF_GPIO_PIN_H0H1,
			NRF_GPIO_PIN_NOSENSE);

	return 0;
}

static void deinitialize_qspi(void)
{
	*(volatile uint32_t *)0x40029010ul = 1ul;
	*(volatile uint32_t *)0x40029054ul = 1ul;

	nrf_gpio_pin_set(NRF_GPIO_PIN_MAP(0, 7));

	if (nrf_qspi_cinstr_long_transfer_is_ongoing(NRF_QSPI)) {
		nrf_qspi_cinstr_long_transfer_continue(NRF_QSPI, NRF_QSPI_CINSTR_LEN_1B, true);
	}
	NRFX_IRQ_DISABLE(QSPI_IRQn);
	nrf_qspi_int_disable(NRF_QSPI, NRF_QSPI_INT_READY_MASK);
	nrf_qspi_task_trigger(NRF_QSPI, NRF_QSPI_TASK_DEACTIVATE);
	nrf_qspi_disable(NRF_QSPI);
	nrf_qspi_event_clear(NRF_QSPI, NRF_QSPI_EVENT_READY);

	nrf_qspi_pins_t pins;
	nrf_qspi_pins_get(NRF_QSPI, &pins);
	nrf_gpio_cfg_default(pins.sck_pin);
	nrf_gpio_cfg_default(pins.io0_pin);
	nrf_gpio_cfg_default(pins.io1_pin);
	if (pins.io2_pin != NRF_QSPI_PIN_NOT_CONNECTED) {
		nrf_gpio_cfg_default(pins.io2_pin);
	}
	if (pins.io3_pin != NRF_QSPI_PIN_NOT_CONNECTED) {
		nrf_gpio_cfg_default(pins.io3_pin);
	}
}

static int set_mode(enum qspi_access_mode mode,
		uint32_t cmd, uint32_t dummy_cycles)
{
	(void)mode;
	(void)cmd;
	(void)dummy_cycles;
	return 0;
}

static int enable_qspi(bool enable)
{
	if (enable) {
		return initialize_qspi();
	}

	/* disable */
	deinitialize_qspi();

	return 0;
}

static int read_reg(uint32_t cmd, void *buf, size_t bytes_to_read)
{
	nrf_qspi_cinstr_conf_t cfg = {
		.opcode    = (uint8_t)cmd,
		.length    = (nrf_qspi_cinstr_len_t)
				(NRF_QSPI_CINSTR_LEN_1B + bytes_to_read),
		.io2_level = true,
		.io3_level = true,
		.wipwait   = false,
		.wren      = false,
	};

	if (nrf_drv_qspi_cinstr_xfer(&cfg, NULL, buf) != NRF_SUCCESS) {
		return -EIO;
	}

	return 0;
}

static int write_reg(uint32_t cmd, bool addr,
		const void *data, size_t bytes_to_write)
{
	nrf_qspi_cinstr_conf_t cfg = {
		.opcode    = (uint8_t)cmd,
		.length    = (nrf_qspi_cinstr_len_t)
				(NRF_QSPI_CINSTR_LEN_1B + bytes_to_write),
		.io2_level = true,
		.io3_level = true,
		.wipwait   = false,
		.wren      = false,
	};

	if (addr) { /* erase */
		if (nrf_drv_qspi_erase(NRF_QSPI_ERASE_LEN_64KB,
				*(uint32_t *)data) != NRF_SUCCESS) {
			return -EIO;
		}

		poll_xfer_done();

		return 0;
	}

	if (nrf_drv_qspi_cinstr_xfer(&cfg, data, NULL) != NRF_SUCCESS) {
		return -EIO;
	}

	return 0;
}

static int write_data(uint32_t cmd, uint32_t addr,
		const void *data, size_t bytes_to_write)
{
	(void)cmd;

	if (nrf_drv_qspi_write(data, bytes_to_write, addr) != NRFX_SUCCESS) {
		return -EIO;
	}

	poll_xfer_done();

	return 0;
}

static int read_data(uint32_t cmd, uint32_t addr,
		void *buf, size_t bytes_to_read,
		uint32_t dummy_cycles)
{
	(void)cmd;
	(void)dummy_cycles;

	if (nrf_drv_qspi_read(buf, bytes_to_read, addr) != NRF_SUCCESS) {
		return -EIO;
	}

	poll_xfer_done();

	return 0;
}

struct qspi *nrf52_qspi_create(void)
{
	static struct qspi io = {
		.read_reg = read_reg,
		.write_reg = write_reg,
		.read = read_data,
		.write = write_data,
		.set_mode = set_mode,
		.enable = enable_qspi,
	};

	return &io;
}
