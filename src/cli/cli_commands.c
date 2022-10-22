/*
 * SPDX-FileCopyrightText: 2021 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "cli_commands.h"

const struct cli_cmd cli_commands[] = {
	{"exit",   cli_cmd_exit,    "Exit the CLI" },
	{"help",   cli_cmd_help,    "List available commands" },
	{"info",   cli_cmd_info,    "Display device info" },
	{"reboot", cli_cmd_reboot,  "Reboot the device" },
	{"md",     cli_cmd_memdump, "Read memory" },
	{"dbg",    cli_cmd_debug,   "Debug functions such as trace" },
	{"wifi",   cli_cmd_wifi,    "WiFi functions" },
	{"mqtt",   cli_cmd_mqtt,    "MQTT functions" },
};

const size_t cli_commands_len = sizeof(cli_commands) / sizeof(*cli_commands);
