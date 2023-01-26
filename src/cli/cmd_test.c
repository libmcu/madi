/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libmcu/compiler.h"

#include "bq25180.h"
#include "bq25180_overrides.h"

#include "battery.h"

static void println(const struct cli_io *io, const char *str)
{
	io->write(str, strlen(str));
	io->write("\n", 1);
}

static void print_help(const struct cli_io *io)
{
	println(io, "subcommands: bq25180");
}

static void print_result(const struct cli_io *io,
		const char *fmt, int reg, int val, int err)
{
	char buf[64];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
	snprintf(buf, sizeof(buf)-1, fmt,
			err<0? "Failed:":"Successfully", val, reg);
#pragma GCC diagnostic pop
	io->write(buf, strlen(buf));
	io->write("\n", 1);
}

static cli_cmd_error_t do_bq25180(int argc, const char *argv[],
		const struct cli_io *io)
{
	uint8_t reg;
	uint8_t val;
	int err;

	if (argc > 3) {
		reg = (uint8_t)strtol(argv[3], NULL, 16);
	}

	if (argc == 3 && strcmp(argv[2], "reset") == 0) {
		bq25180_reset(0);
	} else if (argc == 3 && strcmp(argv[2], "reboot") == 0) {
		bq25180_reset(1);
	} else if (argc == 4 && strcmp(argv[2], "read") == 0) {
		err = bq25180_read(BQ25180_DEVICE_ADDRESS,
				reg, &val, sizeof(val));
		print_result(io, "%s read %x from %x", (int)reg, (int)val, err);
	} else if (argc == 5 && strcmp(argv[2], "write") == 0) {
		val = (uint8_t)strtol(argv[4], NULL, 16);
		err = bq25180_write(BQ25180_DEVICE_ADDRESS,
				reg, &val, sizeof(val));
		print_result(io, "%s written %x into %x", (int)reg, (int)val, err);
	} else {
		println(io, "usage: <read|write> 0xaddr [0xvalue]");
	}

	return CLI_CMD_SUCCESS;
}

static cli_cmd_error_t do_battery(int argc, const char *argv[],
		const struct cli_io *io)
{
	if (argc == 3 && strcmp(argv[2], "init") == 0) {
		battery_enable_monitor(true);
	} else if (argc == 3 && strcmp(argv[2], "level") == 0) {
		int adc = battery_level_raw();
		print_result(io, "%s read %dmV %d",
				0, battery_raw_to_millivolts(adc), 0);
	} else {
		println(io, "usage: <init|level>");
	}

	return CLI_CMD_SUCCESS;
}

DEFINE_CLI_CMD(test, 0) {
	struct cli const *cli = (struct cli const *)env;

	if (argc < 2) {
		print_help(cli->io);
	}

	if (strcmp(argv[1], "help") == 0) {
		print_help(cli->io);
	} else if (strcmp(argv[1], "bq25180") == 0) {
		do_bq25180(argc, argv, cli->io);
	} else if (strcmp(argv[1], "battery") == 0) {
		do_battery(argc, argv, cli->io);
	}

	return CLI_CMD_SUCCESS;
}
