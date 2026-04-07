/*
 * SPDX-FileCopyrightText: 2023 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PINMAP_H
#define PINMAP_H

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(TARGET_PLATFORM_madi_nrf52840)
#include "../ports/nrf52/pinmap.h"
#elif defined(TARGET_PLATFORM_madi_esp32s3)
#include "../ports/esp-idf/boards/madi_esp32s3/pinmap.h"
#elif defined(TARGET_PLATFORM_jc8012wp4a1)
#include "../ports/esp-idf/boards/jc8012wp4a1/pinmap.h"
#else
#error "Unsupported TARGET_PLATFORM"
#endif

struct lm_gpio;
struct pinmap_periph {
	struct lm_gpio *led;
};

/**
 * @brief Initialize the peripheral described by the pinmap.
 *
 * @param[in] periph Pointer to the pinmap_periph structure.
 * @return 0 on success, negative value on error.
 */
int pinmap_init(struct pinmap_periph *periph);

#if defined(__cplusplus)
}
#endif

#endif /* PINMAP_H */
