/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_WIFI_INTERFACE_H
#define FPL_WIFI_INTERFACE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "drivers/net/wifi.h"
#include "util.h"

#if defined(WIFI_DEFAULT_INTERFACE)
#define wifi_create_default	CONCAT(WIFI_DEFAULT_INTERFACE, _wifi_create)
#define wifi_destroy_default	CONCAT(WIFI_DEFAULT_INTERFACE, _wifi_destroy)

struct wifi *wifi_create_default(void);
void wifi_destroy_default(struct wifi *inst);
#else
#define wifi_create_default()	0
#define wifi_destroy_default(x)
#endif

static inline int wifi_connect(struct wifi *self,
			       const struct wifi_conn_param *param)
{
	return self->api.connect(self, param);
}

static inline int wifi_disconnect(struct wifi *self)
{
	return self->api.disconnect(self);
}

static inline int wifi_scan(struct wifi *self)
{
	return self->api.scan(self);
}

static inline int wifi_start(struct wifi *self)
{
	return self->api.start(self);
}

static inline int wifi_stop(struct wifi *self)
{
	return self->api.stop(self);
}

static inline int wifi_get_status(struct wifi *self,
				  struct wifi_iface_info *info)
{
	return self->api.get_status(self, info);
}

static inline int wifi_register_event_callback(struct wifi *self,
					       const wifi_event_callback_t cb)
{
	return self->api.register_event_callback(self, cb);
}

#if defined(__cplusplus)
}
#endif

#endif /* FPL_WIFI_INTERFACE_H */
