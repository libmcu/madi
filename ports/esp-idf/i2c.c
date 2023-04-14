#include "libmcu/i2c.h"
#include "libmcu/port/i2c.h"
#include "libmcu/metrics.h"

#include <errno.h>
#include <string.h>

#include "driver/i2c.h" /* esp-idf */

#define I2C0_SDA_PIN			17
#define I2C0_SCL_PIN			18
#define I2C0_FREQ			100000

struct i2c {
	uint8_t id;
	bool activated;
};

int i2c_port_read(struct i2c *self, uint8_t addr, uint8_t reg,
			   void *buf, size_t bufsize)
{
	if (!self->activated) {
		metrics_increase(I2CError);
		return -EPIPE;
	}

	int rc = i2c_master_write_read_device(self->id,
				     addr, &reg, 1, buf, bufsize, 10);
	if (rc != ESP_OK) {
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

	size_t len = data_len + 1;
	uint8_t *buf = (uint8_t *)malloc(len);
	if (buf == NULL) {
		metrics_increase(HeapAllocFailure);
		return -ENOMEM;
	}

	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	int rc = i2c_master_write_to_device(self->id, addr, buf, len, 10);
	if (rc != ESP_OK) {
		metrics_increase(I2CError);
		rc = -EIO;
	}

	free(buf);

	return rc;
}

struct i2c *i2c_port_create(uint8_t id)
{
	if (id != 0) {
		return NULL;
	}

	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C0_SDA_PIN,
		.scl_io_num = I2C0_SCL_PIN,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C0_FREQ,
	};

	i2c_param_config(id, &conf);
	if (i2c_driver_install(id, conf.mode, 0, 0, 0) != ESP_OK) {
		metrics_increase(I2CError);
		return NULL;
	}

	static struct i2c instance = {
		.activated = true,
	};
	instance.id = id;

	return &instance;
}

void i2c_port_delete(struct i2c *self)
{
	if (self && self->activated && self->id == 0) {
		ESP_ERROR_CHECK(i2c_driver_delete(I2C_NUM_0));
		self->activated = false;
	}
}
