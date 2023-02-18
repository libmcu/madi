/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mxic_nor_qspi.h"
#include <errno.h>

#define READ4IO_DUMMY_CYCLE		6

enum mxic_cmd {
	CMD_WREN	= 0x06U, /**< Write enable */
	CMD_RDSR	= 0x05U, /**< Read status register */
	CMD_WRSR	= 0x01U, /**< Write status register */
	CMD_ERASE_64K	= 0xD8U,
	CMD_ERASE_32K	= 0x52U,
	CMD_PP4IO	= 0x38U, /**< Quad data line */
	CMD_READ4IO	= 0xEBU, /**< Quad data line */
	CMD_PP		= 0x02U, /**< Single data line */
	CMD_READ	= 0x03U, /**< Single data line */
	CMD_RSTEN	= 0x66U, /**< Reset enable */
	CMD_RST		= 0x99U, /**< Reset */
	CMD_DP		= 0xB9U, /**< Deep Powerdown */
};

static int poll_wip(const struct qspi *io)
{
	int rc;
	uint8_t rdsr;

	do { /* wait for programming to be done */
		if ((rc = io->read_reg(CMD_RDSR, &rdsr, sizeof(rdsr))) != 0) {
			goto out;
		}
	} while ((rdsr & 1/*WIP*/) != 0);

out:
	return rc;
}

int mxic_read(const struct qspi *io,
		uint32_t addr, void *buf, size_t bytes_to_read)
{
	int rc;

	if ((rc = poll_wip(io)) != 0) {
		goto out;
	}

	rc = io->read(CMD_READ4IO, addr, buf, bytes_to_read,
			READ4IO_DUMMY_CYCLE);
out:
	return rc;
}

int mxic_write(const struct qspi *io,
		uint32_t addr, const void *data, size_t data_len)
{
	int rc;

	if ((rc = poll_wip(io)) != 0) {
		goto out;
	}
	if ((rc = io->write_reg(CMD_WREN, 0, 0, 0)) != 0) {
		goto out;
	}

	rc = io->write(CMD_PP4IO, addr, data, data_len);
out:
	return rc;
}

int mxic_erase_block_64k(const struct qspi *io, uint32_t addr)
{
	int rc;

	if ((rc = poll_wip(io)) != 0) {
		goto out;
	}
	if ((rc = io->write_reg(CMD_WREN, 0, 0, 0)) != 0) {
		goto out;
	}

	rc = io->write_reg(CMD_ERASE_64K, true, &addr, sizeof(addr));
out:
	return rc;
}

int mxic_set_memory_mapped(const struct qspi *io)
{
	int rc;

	if ((rc = poll_wip(io)) != 0) {
		goto out;
	}

	rc = io->set_mode(QSPI_MEMORY_MAPPED, CMD_READ4IO, READ4IO_DUMMY_CYCLE);
out:
	return rc;
}

int mxic_sync(const struct qspi *io)
{
	return poll_wip(io);
}

int mxic_sleep(const struct qspi *io)
{
	int rc;
	/* FIXME: In nRF5 SDK, read command should be issued first before DP
	 * command. And nRF5 SDK requests the variable to be in RAM with
	 * word-aligned. */
	uint8_t __attribute__((aligned(4))) tmp;

	if ((rc = poll_wip(io)) != 0) {
		goto out;
	}
	if ((rc = io->read(CMD_READ4IO, 0, &tmp, sizeof(tmp),
			READ4IO_DUMMY_CYCLE)) != 0) {
		goto out;
	}

	rc = io->write_reg(CMD_DP, 0, 0, 0);
out:
	return rc;
}

int mxic_reset(const struct qspi *io)
{
	int rc;

	if ((rc = poll_wip(io)) != 0) {
		goto out;
	}
	if ((rc = io->write_reg(CMD_RSTEN, 0, 0, 0)) != 0) {
		goto out;
	}

	rc = io->write_reg(CMD_RST, 0, 0, 0);
out:
	return rc;
}

int mxic_init(const struct qspi *io)
{
	uint8_t rdsr[2] = { 0x40/*quad enable*/, };
	int rc;

	if ((rc = poll_wip(io)) != 0) {
		goto out;
	}
	if ((rc = io->write_reg(CMD_WREN, 0, 0, 0)) != 0) {
		goto out;
	}

	rc = io->write_reg(CMD_WRSR, 0, rdsr, sizeof(rdsr));
out:
	return rc;
}
