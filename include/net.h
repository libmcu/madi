/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NET_H
#define NET_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

struct net_iface_param {
	uint32_t ip;
	uint32_t netmask;
	uint32_t gateway;
};

int net_init(void);
int net_step(void);

#if defined(__cplusplus)
}
#endif

#endif /* NET_H */
