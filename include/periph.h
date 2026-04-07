/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PERIPH_H
#define PERIPH_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
	PERIPH_UI,
} periph_t;

struct pinmap_periph;

/**
 * @brief Initialize the peripheral described by the pinmap.
 *
 * @param[in] periph Pointer to the pinmap_periph structure.
 * @return 0 on success, negative value on error.
 */
int periph_init(struct pinmap_periph *periph);

/**
 * @brief Enable the peripheral described by the pinmap.
 *
 * @param[in] periph Pointer to the pinmap_periph structure.
 * @return 0 on success, negative value on error.
 */
int periph_enable(struct pinmap_periph *periph);

/**
 * @brief Disable the peripheral described by the pinmap.
 *
 * @param[in] periph Pointer to the pinmap_periph structure.
 * @return 0 on success, negative value on error.
 */
int periph_disable(struct pinmap_periph *periph);

/**
 * @brief Enable a specific peripheral type for the given pinmap.
 *
 * @param[in] pinmap Pointer to the pinmap_periph structure.
 * @param[in] periph Peripheral type to enable.
 * @return 0 on success, negative value on error.
 */
int periph_enable_specific(struct pinmap_periph *pinmap, periph_t periph);

/**
 * @brief Disable a specific peripheral type for the given pinmap.
 *
 * @param[in] pinmap Pointer to the pinmap_periph structure.
 * @param[in] periph Peripheral type to disable.
 * @return 0 on success, negative value on error.
 */
int periph_disable_specific(struct pinmap_periph *pinmap, periph_t periph);

/**
 * @brief Reset a specific peripheral type for the given pinmap.
 *
 * @param[in] pinmap Pointer to the pinmap_periph structure.
 * @param[in] periph Peripheral type to reset.
 * @return 0 on success, negative value on error.
 */
int periph_reset_specific(struct pinmap_periph *pinmap, periph_t periph);

#if defined(__cplusplus)
}
#endif

#endif /* PERIPH_H */
