/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_TRANSPORT_INTERFACE_H
#define FPL_TRANSPORT_INTERFACE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>

struct transport_interface {
	int (*connect)(struct transport_interface *self);
	int (*disconnect)(struct transport_interface *self);
	int (*write)(struct transport_interface *self,
			const void *data, size_t data_len);
	int (*read)(struct transport_interface *self,
			void *buf, size_t bufsize);
};

#if defined(__cplusplus)
}
#endif

#endif /* FPL_TRANSPORT_INTERFACE_H */
