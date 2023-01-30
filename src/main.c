/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "libmcu/board.h"
#include "libmcu/logging.h"
#include "libmcu/metrics.h"
#include "libmcu/cli.h"
#include "libmcu/ao.h"
#include "libmcu/ao_timer.h"

#include "status_led.h"
#include "battery.h"
#include "user_button.h"

#define EVENTLOOP_STACK_SIZE_BYTES	4096
#define STATUS_LED_BLINK_INTERVAL_MS	500
#define CLI_MAX_HISTORY			10

enum event {
	EVT_LED,
	EVT_BUTTON,
	EVT_BATTERY,
};

struct ao_event {
	enum event type;
};

static struct ao eventloop;
static struct ao_event evt_led = { .type = EVT_LED };
static struct ao_event evt_button = { .type = EVT_BUTTON };
static struct ao_event evt_battery = { .type = EVT_BATTERY };

static size_t (*stdout)(const void *, size_t);

static void dispatch(struct ao * const ao, const struct ao_event * const event)
{
	switch (event->type) {
	case EVT_LED:
		status_led_toggle();
		ao_post_defer(ao, &evt_led, STATUS_LED_BLINK_INTERVAL_MS);
		break;
	case EVT_BUTTON:
		if (user_button_process()) {
			ao_post_if_unique(ao, &evt_button);
		}
		break;
	case EVT_BATTERY:
		debug("Battery status changed");
		break;
	default:
		break;
	}
}

static void eventloop_init(void)
{
	ao_timer_init();
	ao_create(&eventloop, EVENTLOOP_STACK_SIZE_BYTES, 1);
	ao_start(&eventloop, dispatch);
}

static void on_user_button_state_change(void)
{
	ao_post_if_unique(&eventloop, &evt_button);
}

static void on_battery_status_change(void)
{
	ao_post(&eventloop, &evt_battery);
}

static void shell_start(void)
{
	static char cli_buffer[CLI_CMD_MAXLEN * CLI_MAX_HISTORY];
	struct cli cli;

	DEFINE_CLI_CMD_LIST(cli_commands,
			help, exit, info, reboot, md, metric, wifi, ble, mqtt, test);

	cli_init(&cli, cli_io_create(), cli_buffer, sizeof(cli_buffer));
	cli_register_cmdlist(&cli, cli_commands);
	cli_run(&cli);
}

static size_t logging_stdout_writer(const void *data, size_t size)
{
	unused(size);
	static char buf[LOGGING_MESSAGE_MAXLEN];
	size_t len = logging_stringify(buf, sizeof(buf), data);

	buf[len++] = '\n';
	buf[len] = '\0';

	(*stdout)(buf, len);

	return len;
}

static void logging_stdout_backend_init(void)
{
	static struct logging_backend log_console = { 0, };
	log_console.write = logging_stdout_writer;
	logging_add_backend(&log_console);
}

int main(void)
{
	board_init(); /* should be called very first. */

	metrics_init(0);
	logging_init(board_get_time_since_boot_ms);
	logging_stdout_backend_init();
	stdout = cli_io_create()->write;

	eventloop_init();
	status_led_init();
	battery_init(on_battery_status_change);
	user_button_init(on_user_button_state_change);

	info("[%s] %s %s", board_get_reboot_reason_string(),
			board_get_serial_number_string(),
			board_get_version_string());

	ao_post_defer(&eventloop, &evt_led, STATUS_LED_BLINK_INTERVAL_MS);

	shell_start();

	return 0;
}
