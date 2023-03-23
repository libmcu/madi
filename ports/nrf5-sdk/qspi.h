/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NRF52_QSPI_H
#define NRF52_QSPI_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "pqspi/qspi.h"

struct qspi *nrf52_qspi_create(void);

#if defined(__cplusplus)
}
#endif

#endif
