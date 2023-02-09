/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBMCU_QSPI_H
#define LIBMCU_QSPI_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

enum qspi_access_mode {
	QSPI_INDIRECT,
	QSPI_MEMORY_MAPPED,
};

struct qspi {
	int (*read_reg)(uint32_t cmd, void *buf, size_t bytes_to_read);
	int (*write_reg)(uint32_t cmd, bool addr,
			const void *data, size_t bytes_to_write);

	int (*read)(uint32_t cmd, uint32_t addr,
			void *buf, size_t bytes_to_read,
			uint32_t dummy_cycles);
	int (*write)(uint32_t cmd, uint32_t addr,
			const void *data, size_t bytes_to_write);

	int (*set_mode)(enum qspi_access_mode mode,
			uint32_t cmd, uint32_t dummy_cycles);

	int (*enable)(bool enable);
};

#if !defined(CONCAT)
#define CONCAT_INTERNAL(a, b)	a ## b
#define CONCAT(a, b)		CONCAT_INTERNAL(a, b)
#endif

#define qspi_create_default	CONCAT(TARGET_PLATFORM, _qspi_create)
#define qspi_destroy_default	CONCAT(TARGET_PLATFORM, _qspi_destroy)

struct qspi *qspi_create_default(void);
void qspi_destroy_default(struct qspi *handle);

#if defined(__cplusplus)
}
#endif

#endif /* LIBMCU_QSPI_H */
