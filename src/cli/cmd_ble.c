/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/cli.h"
#include <string.h>
#include <stdlib.h>
#include "pble/ble.h"

static void println(const struct cli_io *io, const char *str)
{
	io->write(str, strlen(str));
	io->write("\n", 1);
}

static void print_help(const struct cli_io *io)
{
	println(io, "subcommands:\n\n\tinit\n\tenable\n\tdisable\n\tadv");
}

static void print_help_adv(const struct cli_io *io)
{
	println(io, "subcommands:\n\n\tinit\n\tstart\n\tstop\n\tname\n\titvl");
}

static void process_adv(struct ble *ble, int argc, const char *argv[],
		const struct cli_io *io)
{
	if (strcmp("init", argv[2]) == 0) {
		ble_adv_init(ble, BLE_ADV_IND);
	} else if (strcmp("start", argv[2]) == 0) {
		if (ble_adv_start(ble) != 0) {
			println(io, "fail");
		}
	} else if (strcmp("stop", argv[2]) == 0) {
		ble_adv_stop(ble);
	} else if (strcmp("name", argv[2]) == 0 && argc == 4) {
		struct ble_adv_payload adv;
		ble_adv_payload_init(&adv);
		ble_adv_payload_add(&adv, 0x09, argv[3], (uint8_t)strlen(argv[3]));
		ble_adv_set_payload(ble, &adv);
	} else if (strcmp("itvl", argv[2]) == 0 && argc == 5) {
		uint16_t min_ms = (uint16_t)strtol(argv[3], NULL, 10);
		uint16_t max_ms = (uint16_t)strtol(argv[4], NULL, 10);
		ble_adv_set_interval(ble, min_ms, max_ms);
	}
}

DEFINE_CLI_CMD(ble, "BLE functions") {
	static struct ble *ble;
	struct cli const *cli = (struct cli const *)env;

	if (argc == 1) {
		print_help(cli->io);
		return CLI_CMD_SUCCESS;
	}

	if (strcmp("help", argv[1]) == 0) {
		print_help(cli->io);
	} else if (strcmp("init", argv[1]) == 0) {
		if (ble) {
			println(cli->io, "already initialized");
		} else {
			ble = ble_create_default();
		}
		goto out;
	}

	if (!ble) {
		println(cli->io, "should be initialized first");
		goto out;
	} else if (strcmp("enable", argv[1]) == 0) {
		ble_enable(ble, BLE_ADDR_PRIVATE_RPA, 0);
	} else if (strcmp("disable", argv[1]) == 0) {
		ble_disable(ble);
	} else if (strcmp("adv", argv[1]) == 0) {
		if (argc == 2) {
			print_help_adv(cli->io);
		} else {
			process_adv(ble, argc, argv, cli->io);
		}
	} else {
		println(cli->io, "Unknown subcommand");
	}

out:
	return CLI_CMD_SUCCESS;
}
