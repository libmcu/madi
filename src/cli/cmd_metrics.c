/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/cli.h"
#include <string.h>
#include <stdio.h>
#include "libmcu/metrics.h"
#include "libmcu/compiler.h"

static void print_metric(metric_key_t key, int32_t value, void *ctx)
{
	struct cli const *cli = (struct cli const *)ctx;
	const char *str = metrics_stringify_key(key);
	char buf[2/*prettier*/+12/*digits*/+1/*null*/] = { 0, };

	if (str == NULL) {
		return;
	}

	cli->io->write(str, strlen(str));
	snprintf(buf, sizeof(buf)-1, ": %ld\n", value);
	cli->io->write(buf, strlen(buf));
}

DEFINE_CLI_CMD(metric, "Show metrics") {
	if (argc == 2) {
		if (strcmp(argv[1], "reset") == 0) {
			metrics_reset();
		}
		return CLI_CMD_SUCCESS;
	} else if (argc != 1) {
		return CLI_CMD_INVALID_PARAM;
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++-compat"
	metrics_iterate(print_metric, CONST_CAST(void *, env));
#pragma GCC diagnostic pop

	return CLI_CMD_SUCCESS;
}
