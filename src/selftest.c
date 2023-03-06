/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "selftest.h"

#include <string.h>
#include "libmcu/board.h"
#include "libmcu/logging.h"
#include "libmcu/timext.h"
#include "libmcu/metrics.h"
#include "libmcu/compiler.h"

#include "bq25180.h"
#include "bq25180_overrides.h"
#include "battery.h"
#include "pble/ble.h"
#include "mxic_nor_qspi.h"
#include "userbutton.h"

static bool test_bq25180(void)
{
	uint8_t val;
	int rc;

	debug("Test BQ25180(I2C)");

	bq25180_reset(0);

	if ((rc = bq25180_read(BQ25180_DEVICE_ADDRESS, 0xC/*MASK_ID*/,
				&val, sizeof(val))) != 0 ||
			val != 0xC0/*default reset value*/) {
		error("%x %d", val, rc);
		return false; 
	}

	return true;
}

static bool test_battery(void)
{
	debug("Test Battery Level(ADC)");

	bq25180_enable_battery_charging(true);

	int mV_charging = battery_raw_to_millivolts(battery_level_raw());

	bq25180_enable_battery_charging(false);

	int mV = battery_raw_to_millivolts(battery_level_raw());

	if (mV_charging < 4000 || mV > 1000) {
		error("%dmV %dmV", mV_charging, mV);
		return false;
	}

	return true;
}

static bool is_battery_attached(void)
{
	if (battery_raw_to_millivolts(battery_level_raw()) < 2800) {
		return false;
	}

	return true;
}

#define QSPI_TEST_DATA_SIZE		256U
#define QSPI_TEST_ADDR			0x1000

static bool test_qspi_flash(void)
{
#if defined(madi_stm32) || defined(madi_nrf52)
	struct qspi *qspi = qspi_create_default();
	const uint8_t *src = (const uint8_t *)0x20000000;
	uint8_t buf[64];

	debug("Test External Flash(QSPI)");

	qspi->enable(true);
	int rc = mxic_reset(qspi);
	rc |= mxic_init(qspi);
	rc |= mxic_erase_block_64k(qspi, QSPI_TEST_ADDR);
	rc |= mxic_write(qspi, QSPI_TEST_ADDR, src, QSPI_TEST_DATA_SIZE);

	for (uint32_t offset = 0; offset < QSPI_TEST_DATA_SIZE;
			offset += sizeof(buf)) {
		rc |= mxic_read(qspi, QSPI_TEST_ADDR + offset, buf, sizeof(buf));
		if (memcmp(&src[offset], buf, sizeof(buf)) != 0) {
			error("not match");
			return false;
		}
	}

	if (rc != 0) {
		error("%d", rc);
		return false;
	}
#endif
	return true;
}

static bool test_ble(void)
{
#if defined(madi_nrf52) || defined(esp32s3) || defined(esp32)
	debug("Test BLE");

	struct ble *ble = ble_create_default();
	ble_enable(ble, BLE_ADDR_PRIVATE_RPA, 0);
	ble_adv_init(ble, BLE_ADV_IND);
	ble_adv_start(ble);
	ble_adv_stop(ble);
#endif
	return true;
}

static bool test_wifi(void)
{
#if defined(esp32) || defined(esp32s3)
#endif
	return true;
}

static void on_userbutton_event(enum button_event event,
		const struct button_data *info, void *ctx)
{
	uint8_t *expected_clicks = (uint8_t *)ctx;

	if (event == BUTTON_EVT_CLICK && info->click == *expected_clicks) {
		*expected_clicks = 0;
	}
}

static void iterate_metrics(metric_key_t key, int32_t value, void *ctx)
{
	unused(key);

	int *abnormal = (int *)ctx;

	if (value != 0) {
		*abnormal += 1;
	}
}

selftest_error_t selftest_button(uint8_t expected_clicks, uint32_t timeout_ms)
{
	uint32_t tout;

	userbutton_set_handler(on_userbutton_event, &expected_clicks);
	timeout_set(&tout, timeout_ms);

	while (!timeout_is_expired(tout) && ACCESS_ONCE(expected_clicks)) {
		/* waiting for user input */
		sleep_ms(100);
	}

	userbutton_unset_handler(on_userbutton_event);

	if (expected_clicks == 0) {
		return SELFTEST_SUCCESS;
	}

	return SELFTEST_ERROR;
}

selftest_error_t selftest(void)
{
	int abnormal = 0;
	selftest_error_t err = SELFTEST_SUCCESS;

	battery_enable_monitor(true);

	if (is_battery_attached()) { /* skip self test */
		err = SELFTEST_BYPASS;
		goto out;
	}

	if (!test_bq25180() ||
			!test_battery() ||
			!test_qspi_flash() ||
			!test_ble() ||
			!test_wifi()) {
		err = SELFTEST_ERROR;
		goto out;
	}

	metrics_iterate(iterate_metrics, &abnormal);

	if (abnormal) {
		err = SELFTEST_ERROR;
	}

out:
	battery_enable_monitor(false);

	return err;
}
