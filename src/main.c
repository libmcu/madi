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
#include "libmcu/ao.h"
#include "libmcu/ao_timer.h"

#include "ledind.h"
#include "userbutton.h"
#include "battery.h"
#include "selftest.h"
#include "net.h"
#include "pusb/usbd.h"

#define AO_STACK_SIZE_BYTES		4096U
#define AO_PRIORITY			1U

#define CLI_MAX_HISTORY			10U

#define LED_BLINK_ON_TIME_MS		50U
#define LED_BLINK_INTERVAL_MS		1500U
#define LED_BLINK_ERROR_INTERVAL_MS	150U

typedef void (*event_handler_t)(void *ctx);

struct ao_event {
	event_handler_t handler;
	void *ctx;
};

static struct ao ao_handle;

static void process_led(void *ctx);
static void process_button(void *ctx);
static void process_battery(void *ctx);

static struct ao_event led_event = {
	.handler = process_led,
};

static struct ao_event button_event = {
	.handler = process_button,
};

static struct ao_event battery_event = {
	.handler = process_battery,
};

static void on_userbutton_state_change(void)
{
	ao_post(&ao_handle, &button_event);
}

static void on_battery_status_change(void)
{
	ao_post(&ao_handle, &battery_event);
}

static void process_button(void *ctx)
{
	unused(ctx);
	if (userbutton_process()) {
		ao_post(&ao_handle, &button_event);
	}
}

static void process_battery(void *ctx)
{
	unused(ctx);
	enum battery_status status = battery_status();

	if (status == BATTERY_UNKNOWN) {
		ao_post_defer(&ao_handle, &battery_event, 100);
	}
}

static void process_led(void *ctx)
{
	unused(ctx);
	uint32_t msec = ledind_step();
	ao_post_defer(&ao_handle, &led_event, msec);
}

static void dispatch(struct ao * const ao, const struct ao_event * const event)
{
	(void)ao;

	if (event && event->handler) {
		(*event->handler)(event->ctx);
	}
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

static void system_init(void)
{
	metrics_init(0);
	logging_init(board_get_time_since_boot_ms);

	ao_timer_init();
	ao_create(&ao_handle, AO_STACK_SIZE_BYTES, AO_PRIORITY);
	ao_start(&ao_handle, dispatch);

	net_init();
	usbd_init();
	usbd_enable();
}

int main(void)
{
	board_init(); /* should be called very first. */
	system_init();

	logging_stdout_backend_init();

	info("[%s] %s %s", board_get_reboot_reason_string(),
			board_get_serial_number_string(),
			board_get_version_string());

	battery_init(battery_monitor_init(on_battery_status_change));
	userbutton_init(userbutton_gpio_init(on_userbutton_state_change));
	ledind_init(ledind_gpio_create());

	ledind_enable();
	ao_post(&ao_handle, &led_event);
	run_selftest();

	shell_start();

	return 0;
}
