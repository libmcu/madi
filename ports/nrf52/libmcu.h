/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBMCU_DEVBOARD_H
#define LIBMCU_DEVBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

#define LEDS_NUMBER				1

#define LED_START				20
#define LED_1					20
#define LED_STOP				20

#define LEDS_ACTIVE_STATE			0

#define LEDS_INV_MASK				LEDS_MASK

#define LEDS_LIST				{ LED_1 }

#define BSP_LED_0				LED_1

#define RX_PIN_NUMBER				25
#define TX_PIN_NUMBER				23
#define CTS_PIN_NUMBER				19
#define RTS_PIN_NUMBER				21

#ifdef __cplusplus
}
#endif

#endif /* LIBMCU_DEVBOARD_H */
