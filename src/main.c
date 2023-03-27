/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>

#include "libmcu/board.h"
#include "libmcu/logging.h"
#include "libmcu/metrics.h"
#include "libmcu/cli.h"
#include "libmcu/syscall.h"

#include "evtloop.h"
#include "ledind.h"
#include "userbutton.h"
#include "battery.h"
#include "selftest.h"

#define EVTLOOP_STACK_SIZE_BYTES	4096U
#define EVTLOOP_PRIORITY		1U

#define CLI_MAX_HISTORY			10U

#define LED_BLINK_ON_TIME_MS		50U
#define LED_BLINK_INTERVAL_MS		1500U
#define LED_BLINK_ERROR_INTERVAL_MS	150U

static void process_led(void *ctx);
static void process_button(void *ctx);
static void process_battery(void *ctx);

static evtloop_event_t led_event = {
	.handler = process_led,
};

static evtloop_event_t button_event = {
	.handler = process_button,
};

static evtloop_event_t battery_event = {
	.handler = process_battery,
};

static void on_userbutton_state_change(void)
{
	evtloop_post(&button_event);
}

static void on_battery_status_change(void)
{
	evtloop_post(&battery_event);
}

static void process_button(void *ctx)
{
	unused(ctx);
	if (userbutton_process()) {
		evtloop_post(&button_event);
	}
}

static void process_battery(void *ctx)
{
	unused(ctx);
	debug("Battery status changed");
}

static void process_led(void *ctx)
{
	unused(ctx);
	uint32_t msec = ledind_step();
	evtloop_post_defer(&led_event, msec);
}

static size_t logging_stdout_writer(const void *data, size_t size)
{
	unused(size);
	static char buf[LOGGING_MESSAGE_MAXLEN];
	size_t len = logging_stringify(buf, sizeof(buf), data);

	buf[len++] = '\n';
	buf[len] = '\0';

	write(1, buf, len);

	return len;
}

static void logging_stdout_backend_init(void)
{
	syscall_register_writer(cli_io_create()->write);

	static struct logging_backend log_console = {
		.write = logging_stdout_writer,
	};

	logging_add_backend(&log_console);
}

static void shell_start(void)
{
	static char cli_buffer[CLI_CMD_MAXLEN * CLI_MAX_HISTORY];
	struct cli cli;

	DEFINE_CLI_CMD_LIST(cli_commands,
			help, exit, info, reboot, md, metric, wifi, ble);

	cli_init(&cli, cli_io_create(), cli_buffer, sizeof(cli_buffer));
	cli_register_cmdlist(&cli, cli_commands);
	cli_run(&cli);
}

static void run_selftest(void)
{
	uint32_t led_blink_interval = LED_BLINK_INTERVAL_MS;

	if (selftest() != SELFTEST_SUCCESS) {
		led_blink_interval = LED_BLINK_ERROR_INTERVAL_MS;
	}

	ledind_set(LEDIND_BLINK, LED_BLINK_ON_TIME_MS,
			led_blink_interval - LED_BLINK_ON_TIME_MS);
}

int main(void)
{
	board_init(); /* should be called very first. */

	metrics_init(0);
	logging_init(board_get_time_since_boot_ms);
	logging_stdout_backend_init();

	evtloop_init(EVTLOOP_PRIORITY, EVTLOOP_STACK_SIZE_BYTES);
	battery_init(battery_monitor_init(on_battery_status_change));
	userbutton_init(userbutton_gpio_init(on_userbutton_state_change));
	ledind_init(ledind_gpio_create());

	info("[%s] %s %s", board_get_reboot_reason_string(),
			board_get_serial_number_string(),
			board_get_version_string());

	ledind_enable();
	evtloop_post(&led_event);
	run_selftest();

	shell_start();

	return 0;
}
