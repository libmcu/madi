/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LIBMCU_I2C_INTERFACE_H
#define LIBMCU_I2C_INTERFACE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "pi2c/i2c.h"

static inline int i2c_init(struct i2c *self)
{
	return ((struct i2c_api *)self)->init(self);
}

static inline int i2c_deinit(struct i2c *self)
{
	return ((struct i2c_api *)self)->deinit(self);
}

static inline int i2c_write(struct i2c *self, uint8_t addr, uint8_t reg,
			    const void *data, size_t data_len)
{
	return ((struct i2c_api *)self)->write(self, addr, reg, data, data_len);
}

static inline int i2c_read(struct i2c *self, uint8_t addr, uint8_t reg,
			   void *buf, size_t bufsize)
{
	return ((struct i2c_api *)self)->read(self, addr, reg, buf, bufsize);
}

#if defined(__cplusplus)
}
#endif

#endif /* LIBMCU_I2C_INTERFACE_H */
