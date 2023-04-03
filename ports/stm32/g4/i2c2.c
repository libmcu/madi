#include "i2c2.h"
#include <errno.h>
#include <string.h>
#include "i2c.h"
#include "libmcu/metrics.h"

static int i2c2_read(struct i2c *self, uint8_t addr, uint8_t reg,
			   void *buf, size_t bufsize)
{
	(void)self;

	addr <<= 1;

	if (HAL_I2C_Master_Transmit(&hi2c2, addr, &reg, 1, HAL_MAX_DELAY)
			!= HAL_OK) {
		metrics_increase(I2CError);
		return -EIO;
	}

	if (HAL_I2C_Master_Receive(&hi2c2, addr, buf, bufsize, HAL_MAX_DELAY)
			!= HAL_OK) {
		metrics_increase(I2CError);
		return -EIO;
	}

	return 0;
}

static int i2c2_write(struct i2c *self, uint8_t addr, uint8_t reg,
			    const void *data, size_t data_len)
{
	(void)self;

	addr <<= 1;

	uint8_t buf[data_len + 1];
	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	if (HAL_I2C_Master_Transmit(&hi2c2, addr, buf, data_len+1, HAL_MAX_DELAY)
			!= HAL_OK) {
		metrics_increase(I2CError);
		return -EIO;
	}

	return 0;
}

static int i2c2_init(struct i2c *self)
{
	MX_I2C2_Init();
	return 0;
}

static int i2c2_deinit(struct i2c *self)
{
	MX_I2C2_DeInit();
	return 0;
}

struct i2c *i2c2_create(void)
{
	static struct i2c_api iface = {
		.init = i2c2_init,
		.deinit = i2c2_deinit,
		.write = i2c2_write,
		.read = i2c2_read,
	};

	return (struct i2c *)&iface;
}

void i2c2_delete(struct i2c *self)
{
	(void)self;
}
