/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NRF52_I2C0_H
#define NRF52_I2C0_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "pi2c/i2c.h"

struct i2c *i2c0_create(void);
void i2c0_delete(struct i2c *self);

#if defined(__cplusplus)
}
#endif

#endif
