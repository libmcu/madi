/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "i2c0.h"
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "nrf.h"
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "bsp.h"

#define I2C0_SDA_PIN		45 /*P1.13*/
#define I2C0_SCL_PIN		44 /*P1.12*/

#define I2C0_FREQ		NRF_DRV_TWI_FREQ_100K

#define IRQ_PRIO		APP_IRQ_PRIORITY_HIGH

static const nrf_drv_twi_t handle = NRF_DRV_TWI_INSTANCE(0);
static volatile bool is_xfer_done;

static void on_event(nrf_drv_twi_evt_t const *p_event, void *p_context)
{
	switch (p_event->type) {
	case NRF_DRV_TWI_EVT_DONE:
		if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX) {
		}

		is_xfer_done = true;
		break;
	default:
		break;
	}
}

static void prepare_xfer(void)
{
	is_xfer_done = false;
}

static void finish_xfer(void)
{
	while (!is_xfer_done) {
		/* wait for transfer to be done */
	}
}

static int i2c0_read(struct i2c *self, uint8_t addr, uint8_t reg,
			   void *buf, size_t bufsize)
{
	(void)self;

	prepare_xfer();

	if (nrf_drv_twi_tx(&handle, addr, &reg, sizeof(reg), true)
			!= NRF_SUCCESS) {
		return -EIO;
	}

	finish_xfer();
	prepare_xfer();

	if (nrf_drv_twi_rx(&handle, addr, buf, bufsize) != NRF_SUCCESS) {
		return -EIO;
	}

	finish_xfer();

	return bufsize;
}

static int i2c0_write(struct i2c *self, uint8_t addr, uint8_t reg,
			    const void *data, size_t data_len)
{
	(void)self;

	uint8_t buf[data_len + 1];
	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	prepare_xfer();

	if (nrf_drv_twi_tx(&handle, addr, buf, (uint8_t)(data_len+1), false)
			!= NRF_SUCCESS) {
		return -EIO;
	}

	finish_xfer();

	return data_len;
}

static int i2c0_init(struct i2c *self)
{
	(void)self;

	const nrf_drv_twi_config_t cfg = {
		.scl = I2C0_SCL_PIN,
		.sda = I2C0_SDA_PIN,
		.frequency = I2C0_FREQ,
		.interrupt_priority = APP_IRQ_PRIORITY_HIGH,
		.clear_bus_init = false,
	};

	if (nrf_drv_twi_init(&handle, &cfg, on_event, NULL) != NRF_SUCCESS) {
		return -EIO;
	}

	nrf_drv_twi_enable(&handle);

	return 0;
}

static int i2c0_deinit(struct i2c *self)
{
	(void)self;
	return 0;
}

struct i2c *i2c0_create(void)
{
	static struct i2c_api iface = {
		.init = i2c0_init,
		.deinit = i2c0_deinit,
		.write = i2c0_write,
		.read = i2c0_read,
	};

	return (struct i2c *)&iface;
}

void i2c0_delete(struct i2c *self)
{
	(void)self;
}
