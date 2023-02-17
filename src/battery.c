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

#define NR_SAMPLES		20

static const struct battery_monitor *monitor;
static struct i2c *i2c_handle;

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

int battery_level_raw(void)
{
	int samples[NR_SAMPLES];

	for (int i = 0; i < NR_SAMPLES; i++) {
		samples[i] = monitor->get_level_adc();
	}

	int avg = calc_average(samples, NR_SAMPLES);
	int var = calc_variance(samples, NR_SAMPLES, avg);
	avg = calc_average_filtered(samples, NR_SAMPLES, avg, var);

	return avg;
}

int battery_raw_to_millivolts(int raw)
{
	return monitor->adc_to_millivolts(raw);
}

int battery_enable_monitor(bool enable)
{
	return monitor->enable(enable);
}

int battery_init(const struct battery_monitor *battery_monitor)
{
	monitor = battery_monitor;

	bq25180_reset(false);
	bq25180_enable_push_button(false);
	bq25180_enable_battery_charging(false);
	bq25180_enable_interrupt(BQ25180_INTR_CHARGING_STATUS);
	bq25180_set_watchdog_timer(BQ25180_WDT_DISABLE);
	bq25180_enable_vindpm(BQ25180_VINDPM_4700mV);
	bq25180_set_sys_voltage(BQ25180_SYS_REG_PASS_THROUGH);

	return 0;
}
