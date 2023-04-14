/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
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

#define MAX_I2C			2

#if !defined(I2C0_SDA_PIN)
#define I2C0_SDA_PIN		45 /*P1.13*/
#endif
#if !defined(I2C0_SCL_PIN)
#define I2C0_SCL_PIN		44 /*P1.12*/
#endif
#if !defined(I2C0_FREQ)
#define I2C0_FREQ		NRF_DRV_TWI_FREQ_100K
#endif
#if !defined(I2C1_SDA_PIN)
#define I2C1_SDA_PIN		45
#endif
#if !defined(I2C1_SCL_PIN)
#define I2C1_SCL_PIN		44
#endif
#if !defined(I2C1_FREQ)
#define I2C1_FREQ		NRF_DRV_TWI_FREQ_100K
#endif

struct i2c {
	uint8_t channel;
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
	if (!self || !self->activated) {
		metrics_increase(I2CErrorPipe);
		return -EPIPE;
	}

	prepare_xfer(self);

	if (nrf_drv_twi_tx(&self->handle, addr, &reg, sizeof(reg), true)
			!= NRF_SUCCESS) {
		metrics_increase(I2CErrorIO);
		return -EIO;
	}

	finish_xfer(self);
	prepare_xfer(self);

	if (nrf_drv_twi_rx(&self->handle, addr, (uint8_t *)buf,
				(uint8_t)bufsize) != NRF_SUCCESS) {
		metrics_increase(I2CErrorIO);
		return -EIO;
	}

	finish_xfer(self);

	return 0;
}

int i2c_port_write(struct i2c *self, uint8_t addr, uint8_t reg,
			    const void *data, size_t data_len)
{
	if (!self || !self->activated) {
		metrics_increase(I2CErrorPipe);
		return -EPIPE;
	}

	uint8_t buf[data_len + 1];
	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	prepare_xfer(self);

	if (nrf_drv_twi_tx(&self->handle, addr, buf, (uint8_t)(data_len+1),
				false) != NRF_SUCCESS) {
		metrics_increase(I2CErrorIO);
		return -EIO;
	}

	finish_xfer(self);

	return 0;
}

int i2c_port_enable(struct i2c *self)
{
	if (!self) {
		metrics_increase(I2CErrorPipe);
		return -EPIPE;
	} else if (self->activated) {
		metrics_increase(I2CErrorSequence);
		return -EALREADY;
	}

	nrf_drv_twi_config_t cfg = {
		.scl = I2C0_SCL_PIN,
		.sda = I2C0_SDA_PIN,
		.frequency = I2C0_FREQ,
		.interrupt_priority = APP_IRQ_PRIORITY_HIGH,
		.clear_bus_init = false,
	};

	if (self->channel == 1) {
		self->handle = (nrf_drv_twi_t)NRF_DRV_TWI_INSTANCE(1);

		cfg.sda = I2C1_SDA_PIN;
		cfg.scl = I2C1_SCL_PIN;
		cfg.frequency = I2C1_FREQ;
	} else {
		self->handle = (nrf_drv_twi_t)NRF_DRV_TWI_INSTANCE(0);
	}

	if (nrf_drv_twi_init(&self->handle, &cfg, on_event, self)
			!= NRF_SUCCESS) {
		metrics_increase(I2CFault);
		return -EFAULT;
	}

	nrf_drv_twi_enable(&self->handle);
	self->activated = true;

	return 0;
}

int i2c_port_disable(struct i2c *self)
{
	if (!self) {
		metrics_increase(I2CErrorPipe);
		return -EPIPE;
	} else if (!self->activated) {
		metrics_increase(I2CErrorSequence);
		return -EALREADY;
	}

	nrf_drv_twi_disable(&self->handle);
	nrf_drv_twi_uninit(&self->handle);

	self->activated = false;

	return 0;
}

struct i2c *i2c_port_create(uint8_t channel)
{
	static struct i2c i2c[MAX_I2C];

	if (channel >= MAX_I2C || i2c[channel].activated) {
		return NULL;
	}

	i2c[channel].channel = channel;

	return &i2c[channel];
}

void i2c_port_delete(struct i2c *self)
{
	memset(self, 0, sizeof(*self));
}
