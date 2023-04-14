/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/i2c.h"
#include "libmcu/port/i2c.h"
#include "libmcu/metrics.h"

#include <errno.h>
#include <string.h>

#include "driver/i2c.h" /* esp-idf */

#define MAX_I2C				2

#if !defined(I2C0_SDA_PIN)
#define I2C0_SDA_PIN			17
#endif
#if !defined(I2C0_SCL_PIN)
#define I2C0_SCL_PIN			18
#endif
#if !defined(I2C0_FREQ)
#define I2C0_FREQ			100000
#endif
#if !defined(I2C1_SDA_PIN)
#define I2C1_SDA_PIN			17
#endif
#if !defined(I2C1_SCL_PIN)
#define I2C1_SCL_PIN			18
#endif
#if !defined(I2C1_FREQ)
#define I2C1_FREQ			100000
#endif

struct i2c {
	uint8_t channel;
	bool activated;
};

int i2c_port_read(struct i2c *self, uint8_t addr, uint8_t reg,
			   void *buf, size_t bufsize)
{
	if (!self || !self->activated) {
		metrics_increase(I2CErrorPipe);
		return -EPIPE;
	}

	int rc = i2c_master_write_read_device(self->channel,
				     addr, &reg, 1, buf, bufsize, 10);
	if (rc != ESP_OK) {
		metrics_increase(I2CErrorIO);
		return -EIO;
	}

	return 0;
}

int i2c_port_write(struct i2c *self, uint8_t addr, uint8_t reg,
			    const void *data, size_t data_len)
{
	if (!self || !self->activated) {
		metrics_increase(I2CErrorPipe);
		return -EPIPE;
	}

	size_t len = data_len + 1;
	uint8_t *buf = (uint8_t *)malloc(len);
	if (buf == NULL) {
		metrics_increase(HeapAllocFailure);
		return -ENOMEM;
	}

	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	int rc = i2c_master_write_to_device(self->channel, addr, buf, len, 10);
	if (rc != ESP_OK) {
		metrics_increase(I2CErrorIO);
		rc = -EIO;
	}

	free(buf);

	return rc;
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

	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C0_SDA_PIN,
		.scl_io_num = I2C0_SCL_PIN,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C0_FREQ,
	};

	if (self->channel == 1) {
		conf.sda_io_num = I2C1_SDA_PIN;
		conf.scl_io_num = I2C1_SCL_PIN;
		conf.master.clk_speed = I2C1_FREQ;
	}

	if (i2c_param_config(self->channel, &conf) != ESP_OK) {
		metrics_increase(I2CErrorParam);
		return -EINVAL;
	}

	if (i2c_driver_install(self->channel, conf.mode, 0, 0, 0) != ESP_OK) {
		metrics_increase(I2CFault);
		return -EFAULT;
	}

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

	if (i2c_driver_delete(self->channel) != ESP_OK) {
		metrics_increase(I2CFault);
		return -EFAULT;
	}

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
