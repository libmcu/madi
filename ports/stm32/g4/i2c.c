#include "libmcu/i2c.h"
#include "libmcu/port/i2c.h"
#include "libmcu/metrics.h"

#include <errno.h>
#include <string.h>

#include "i2c.h"

struct i2c {
	I2C_HandleTypeDef *handle;
	bool activated;
};

int i2c_port_read(struct i2c *self, uint8_t addr, uint8_t reg,
			   void *buf, size_t bufsize)
{
	if (!self->activated) {
		metrics_increase(I2CError);
		return -EPIPE;
	}

	addr <<= 1;

	if (HAL_I2C_Master_Transmit(self->handle, addr, &reg, 1,
				HAL_MAX_DELAY) != HAL_OK) {
		metrics_increase(I2CError);
		return -EIO;
	}

	if (HAL_I2C_Master_Receive(self->handle, addr, (uint8_t *)buf,
				(uint16_t)bufsize, HAL_MAX_DELAY) != HAL_OK) {
		metrics_increase(I2CError);
		return -EIO;
	}

	return 0;
}

int i2c_port_write(struct i2c *self, uint8_t addr, uint8_t reg,
			    const void *data, size_t data_len)
{
	if (!self->activated) {
		metrics_increase(I2CError);
		return -EPIPE;
	}

	addr <<= 1;

	uint8_t buf[data_len + 1];
	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	if (HAL_I2C_Master_Transmit(self->handle, addr, buf,
			(uint16_t)(data_len+1), HAL_MAX_DELAY) != HAL_OK) {
		metrics_increase(I2CError);
		return -EIO;
	}

	return 0;
}

struct i2c *i2c_port_create(uint8_t id)
{
	if (id != 2) {
		return NULL;
	}

	static struct i2c instance = {
		.handle = &hi2c2,
		.activated = true,
	};

	MX_I2C2_Init();

	return &instance;
}

void i2c_port_delete(struct i2c *self)
{
	if (self && self->activated && self->handle == &hi2c2) {
		MX_I2C2_DeInit();
		self->activated = false;
	}
}
