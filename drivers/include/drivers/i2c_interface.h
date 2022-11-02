/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_I2C_INTERFACE_H
#define FPL_I2C_INTERFACE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

struct i2c_interface {
	int (*init)(struct i2c_interface *self);
	int (*read)(struct i2c_interface *self, uint8_t addr, uint8_t reg,
			void *buf, size_t bufsize);
	int (*write)(struct i2c_interface *self, uint8_t addr, uint8_t reg,
			const void *data, size_t data_len);
	int (*deinit)(struct i2c_interface *self);
};

#if defined(__cplusplus)
}
#endif

#endif /* FPL_I2C_INTERFACE_H */
