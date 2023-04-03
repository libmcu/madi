/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "battery.h"
#include "bq25180.h"

#include "bq25180_overrides.h"
#if defined(madi_stm32)
#include "i2c2.h"
#else
#include "i2c0.h"
#endif

#include <pthread.h>

#if !defined(BATTERY_MIN_MILLIVOLTS)
#define BATTERY_MIN_MILLIVOLTS		3000U
#endif
#if !defined(BATTERY_MAX_MILLIVOLTS)
#define BATTERY_MAX_MILLIVOLTS		4200U
#endif

#define NR_SAMPLES			20

#if !defined(MAX)
#define MAX(a, b)			(((a) < (b))? (b) : (a))
#endif

static const struct battery_monitor *monitor;
static pthread_mutex_t monitor_lock;
static struct i2c *i2c_handle;
static int reference_count;

static int calc_average(const int *samples, int n)
{
	int sum = 0;

	for (int i = 0; i < n; i++) {
		sum += samples[i];
	}

	return sum / n;
}

static int calc_variance(const int *samples, int n, int avg)
{
	int sum = 0;

	for (int i = 0; i < n; i++) {
		int diff = samples[i] - avg;
		sum += diff * diff;
	}

	return sum / n;
}

static int calc_average_filtered(const int *samples, int n, int avg, int var)
{
	int c = 0;
	int sum = 0;

	for (int i = 0; i < n; i++) {
		int diff = samples[i] - avg;

		if ((diff * diff) > var) {
			continue;
		}

		sum += samples[i];
		c++;
	}

	return sum / c;
}

static void initialize_i2c(void)
{
#if defined(madi_stm32)
	i2c_handle = i2c2_create();
#else
	i2c_handle = i2c0_create();
#endif
	i2c_init(i2c_handle);
}

static void read_samples(int *samples, int n)
{
	for (int i = 0; i < n; i++) {
		samples[i] = monitor->get_level_adc();
	}
}

static int enable_monitor(bool enable)
{
	int rc = 0;
	int cnt = enable? 1 : -1;

	pthread_mutex_lock(&monitor_lock);
	reference_count += cnt;
	if ((enable && reference_count == 1) ||
		(!enable && reference_count == 0)) {
		rc = monitor->enable(enable);
	}
	pthread_mutex_unlock(&monitor_lock);

	return rc;
}

static int read_sample_mean(void)
{
	int samples[NR_SAMPLES];
	int n = (int)(sizeof(samples) / sizeof(*samples));

	pthread_mutex_lock(&monitor_lock);

	read_samples(samples, n);

	int avg = calc_average(samples, n);
	int var = calc_variance(samples, n, avg);
	avg = calc_average_filtered(samples, n, avg, var);

	pthread_mutex_unlock(&monitor_lock);

	return avg;
}

static int read_raw(void)
{
	enable_monitor(true);
	int val = read_sample_mean();
	enable_monitor(false);

	return val;
}

static int sample_mean_to_millivolts(int sample_mean)
{
	int millivolts;

	enable_monitor(true);

	pthread_mutex_lock(&monitor_lock);
	millivolts = monitor->adc_to_millivolts(sample_mean);
	pthread_mutex_unlock(&monitor_lock);

	enable_monitor(false);

	return millivolts;
}

int bq25180_read(uint8_t addr, uint8_t reg, void *buf, size_t bufsize)
{
	if (!i2c_handle) {
		initialize_i2c();
	}
	return i2c_read(i2c_handle, addr, reg, buf, bufsize);
}

int bq25180_write(uint8_t addr, uint8_t reg, const void *data, size_t data_len)
{
	if (!i2c_handle) {
		initialize_i2c();
	}
	return i2c_write(i2c_handle, addr, reg, data, data_len);
}

uint8_t battery_level_pct(void)
{
	int millivolts = sample_mean_to_millivolts(read_raw());

	millivolts = MAX(millivolts, (int)BATTERY_MIN_MILLIVOLTS);
	return (uint8_t)
		(((uint32_t)millivolts - BATTERY_MIN_MILLIVOLTS) * 100 /
		(BATTERY_MAX_MILLIVOLTS - BATTERY_MIN_MILLIVOLTS));
}

int battery_level_raw(void)
{
	return read_raw();
}

int battery_raw_to_millivolts(int raw)
{
	return sample_mean_to_millivolts(raw);
}

int battery_enable_monitor(bool enable)
{
	return enable_monitor(enable);
}

battery_status_t battery_status(void)
{
	struct bq25180_state state = { 0, };
	struct bq25180_event event = { 0, };
	battery_status_t rc;

	if (!bq25180_read_event(&event) || !bq25180_read_state(&state)) {
		return BATTERY_UNKNOWN;
	}

	switch (state.charging_status) {
	case 1: /* current charging */
		/* fall through */
	case 2: /* voltage charging */
		rc = BATTERY_CHARGING;
		break;
	case 3: /* done or disabled by host */
		rc = BATTERY_CHARGED;
		break;
	default: /* fall through */
	case 0: /* not charging */
		rc = BATTERY_CONNECTED;
		break;
	}

	bq25180_read_event(&event);

#if 0 /* TODO: detect battery detachment */
	int bat_millivolts = 0;
	if (rc == BATTERY_CHARGED && state.vin_good) {
		bq25180_enable_battery_charging(false);
		enable_monitor(true);
		bat_millivolts = battery_raw_to_millivolts(battery_level_raw());
		enable_monitor(false);
		bq25180_enable_battery_charging(true);

		if (bat_millivolts < 2800) {
			rc = BATTERY_DISCONNECTED;
		}
	}
	debug("(%dmillivolts) state %x, event %x", bat_millivolts, state, event);
#endif

	return rc;
}

void battery_enable_charging(void)
{
	bq25180_enable_battery_charging(true);
}

void battery_disable_charging(void)
{
	bq25180_enable_battery_charging(false);
}

int battery_init(const struct battery_monitor *battery_monitor)
{
	pthread_mutex_init(&monitor_lock, NULL);

	reference_count = 0;
	monitor = battery_monitor;

	bq25180_reset(false);
	bq25180_enable_push_button(false);
	bq25180_enable_interrupt(BQ25180_INTR_ALL);
	bq25180_enable_vindpm(BQ25180_VINDPM_4700mV);
	bq25180_set_watchdog_timer(BQ25180_WDT_DISABLE);
	bq25180_set_sys_voltage(BQ25180_SYS_REG_PASS_THROUGH);
	bq25180_set_battery_regulation_voltage(BATTERY_MAX_MILLIVOLTS);
	bq25180_set_precharge_threshold(BATTERY_MIN_MILLIVOLTS);

	return 0;
}
