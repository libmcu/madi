#include "libmcu/i2c.h"
#include "libmcu/port/i2c.h"
#include "libmcu/metrics.h"

#include <errno.h>
#include <string.h>

#include "i2c.h"

#define MAX_I2C				1

struct i2c {
	I2C_HandleTypeDef *handle;
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

	addr <<= 1;

	if (HAL_I2C_Master_Transmit(self->handle, addr, &reg, 1,
				HAL_MAX_DELAY) != HAL_OK) {
		metrics_increase(I2CErrorIO);
		return -EIO;
	}

	if (HAL_I2C_Master_Receive(self->handle, addr, (uint8_t *)buf,
				(uint16_t)bufsize, HAL_MAX_DELAY) != HAL_OK) {
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

	addr <<= 1;

	uint8_t buf[data_len + 1];
	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	if (HAL_I2C_Master_Transmit(self->handle, addr, buf,
			(uint16_t)(data_len+1), HAL_MAX_DELAY) != HAL_OK) {
		metrics_increase(I2CErrorIO);
		return -EIO;
	}

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

	MX_I2C2_Init();
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

	MX_I2C2_DeInit();
	self->activated = false;

	return 0;
}

struct i2c *i2c_port_create(uint8_t channel)
{
	static struct i2c i2c[MAX_I2C];

	if (channel != 2 || i2c[0].activated) {
		return NULL;
	}

	i2c[0].channel = channel;
	i2c[0].handle = &hi2c2;

	return &i2c[0];
}

void i2c_port_delete(struct i2c *self)
{
	memset(self, 0, sizeof(*self));
}
