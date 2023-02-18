/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBMCU_MXIC_NOR_H
#define LIBMCU_MXIC_NOR_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "pqspi/qspi.h"

int mxic_init(const struct qspi *io);
int mxic_reset(const struct qspi *io);
int mxic_sync(const struct qspi *io);
int mxic_sleep(const struct qspi *io);
int mxic_read(const struct qspi *io,
		uint32_t addr, void *buf, size_t bytes_to_read);
int mxic_write(const struct qspi *io,
		uint32_t addr, const void *data, size_t data_len);
int mxic_erase_sector(const struct qspi *io, uint32_t addr);
int mxic_erase_block_32k(const struct qspi *io, uint32_t addr);
int mxic_erase_block_64k(const struct qspi *io, uint32_t addr);
int mxic_set_memory_mapped(const struct qspi *io);

#if defined(__cplusplus)
}
#endif

#endif /* LIBMCU_MXIC_NOR_H */
