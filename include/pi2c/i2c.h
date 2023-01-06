/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LIBMCU_I2C_H
#define LIBMCU_I2C_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

struct i2c;

struct i2c_api {
	int (*init)(struct i2c *self);
	int (*read)(struct i2c *self, uint8_t addr, uint8_t reg,
			void *buf, size_t bufsize);
	int (*write)(struct i2c *self, uint8_t addr, uint8_t reg,
			const void *data, size_t data_len);
	int (*deinit)(struct i2c *self);
};

#include "pi2c/i2c_interface.h"

#if defined(__cplusplus)
}
#endif

#endif /* LIBMCU_I2C_H */
