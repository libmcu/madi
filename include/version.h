/*
 * SPDX-FileCopyrightText: 2026 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef VERSION_H
#define VERSION_H

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(MAKE_APP_VERSION)
#define MAKE_APP_VERSION(major, minor, patch)	\
	(((major) << 16) | ((minor) << 8) | (patch))
#endif

#if !defined(GET_VERSION_MAJOR)
#define GET_VERSION_MAJOR(version)		(((version) >> 16) & 0xff)
#endif
#if !defined(GET_VERSION_MINOR)
#define GET_VERSION_MINOR(version)		(((version) >> 8) & 0xff)
#endif
#if !defined(GET_VERSION_PATCH)
#define GET_VERSION_PATCH(version)		((version) & 0xff)
#endif

#define APP_VERSION_MAJOR			0
#define APP_VERSION_MINOR			1
#define APP_VERSION_PATCH			0

#define APP_VERSION				\
	MAKE_APP_VERSION(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH)

#if defined(__cplusplus)
}
#endif

#endif /* VERSION_H */
