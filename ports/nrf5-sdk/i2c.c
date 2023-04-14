/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/i2c.h"
#include "libmcu/port/i2c.h"
#include "libmcu/metrics.h"
#include "libmcu/timext.h"

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

struct i2c {
	nrf_drv_twi_t handle;
	volatile bool is_xfer_done;
	bool activated;
};

static void on_event(nrf_drv_twi_evt_t const *p_event, void *p_context)
{
	struct i2c *instance = (struct i2c *)p_context;

	switch (p_event->type) {
	case NRF_DRV_TWI_EVT_DONE:
		if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX) {
		}

		if (instance) {
			instance->is_xfer_done = true;
		}
		break;
	default:
		break;
	}
}

static void prepare_xfer(struct i2c *self)
{
	uint32_t tout;
	timeout_set(&tout, 10);
	while (nrf_drv_twi_is_busy(&self->handle) &&
			!timeout_is_expired(tout)) {
		/* waiting to be available */
	}

	self->is_xfer_done = false;
}

static void finish_xfer(struct i2c *self)
{
	uint32_t tout;
	timeout_set(&tout, 1000);
	while (!self->is_xfer_done && !timeout_is_expired(tout)) {
		/* wait for transfer to be done */
	}
}

int i2c_port_read(struct i2c *self, uint8_t addr, uint8_t reg,
			   void *buf, size_t bufsize)
{
	if (!self->activated) {
		metrics_increase(I2CError);
		return -EPIPE;
	}

	prepare_xfer(self);

	if (nrf_drv_twi_tx(&self->handle, addr, &reg, sizeof(reg), true)
			!= NRF_SUCCESS) {
		metrics_increase(I2CError);
		return -EIO;
	}

	finish_xfer(self);
	prepare_xfer(self);

	if (nrf_drv_twi_rx(&self->handle, addr, (uint8_t *)buf,
				(uint8_t)bufsize) != NRF_SUCCESS) {
		metrics_increase(I2CError);
		return -EIO;
	}

	finish_xfer(self);

	return 0;
}

int i2c_port_write(struct i2c *self, uint8_t addr, uint8_t reg,
			    const void *data, size_t data_len)
{
	if (!self->activated) {
		metrics_increase(I2CError);
		return -EPIPE;
	}

	uint8_t buf[data_len + 1];
	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	prepare_xfer(self);

	if (nrf_drv_twi_tx(&self->handle, addr, buf, (uint8_t)(data_len+1),
				false) != NRF_SUCCESS) {
		metrics_increase(I2CError);
		return -EIO;
	}

	finish_xfer(self);

	return 0;
}

struct i2c *i2c_port_create(uint8_t id)
{
	if (id != 0) {
		return NULL;
	}

	static struct i2c instance = {
		.handle = NRF_DRV_TWI_INSTANCE(0),
	};

	const nrf_drv_twi_config_t cfg = {
		.scl = I2C0_SCL_PIN,
		.sda = I2C0_SDA_PIN,
		.frequency = I2C0_FREQ,
		.interrupt_priority = APP_IRQ_PRIORITY_HIGH,
		.clear_bus_init = false,
	};

	if (nrf_drv_twi_init(&instance.handle, &cfg, on_event, &instance)
			!= NRF_SUCCESS) {
		metrics_increase(I2CError);
		return NULL;
	}

	nrf_drv_twi_enable(&instance.handle);
	instance.activated = true;

	return &instance;
}

void i2c_port_delete(struct i2c *self)
{
	(void)self;
}
