#include "i2c0.h"
#include <errno.h>
#include <string.h>
#include "driver/i2c.h" /* esp-idf */

#define I2C0_SDA_PIN			17
#define I2C0_SCL_PIN			18
#define I2C0_FREQ			100000

static int i2c0_read(struct i2c *self, uint8_t addr, uint8_t reg,
			   void *buf, size_t bufsize)
{
	(void)self;
	int rc = i2c_master_write_read_device(I2C_NUM_0,
				     addr, &reg, 1, buf, bufsize, 0);
	if (rc != ESP_OK) {
		return -EAGAIN;
	}

	return bufsize;
}

static int i2c0_write(struct i2c *self, uint8_t addr, uint8_t reg,
			    const void *data, size_t data_len)
{
	(void)self;
	size_t len = data_len + 1;
	uint8_t *buf = (uint8_t *)malloc(len);
	if (buf == NULL) {
		return -ENOMEM;
	}

	buf[0] = reg;
	memcpy(&buf[1], data, data_len);

	int rc =  i2c_master_write_to_device(I2C_NUM_0, addr, buf, len, 0);

	if (rc != ESP_OK) {
		return -EAGAIN;
	}

	return data_len;
}

static int i2c0_init(struct i2c *self)
{
	(void)self;

	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C0_SDA_PIN,
		.scl_io_num = I2C0_SCL_PIN,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C0_FREQ,
	};

	i2c_param_config(I2C_NUM_0, &conf);

	return i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
}

static int i2c0_deinit(struct i2c *self)
{
	(void)self;
	ESP_ERROR_CHECK(i2c_driver_delete(I2C_NUM_0));
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
