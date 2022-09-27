/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "common/wifi.h"
#include <errno.h>
#include "libmcu/compiler.h"

static void raise_event_with_data(wifi_iface_t iface,
				  enum wifi_event evt, const void *data)
{
	if (iface->callbacks) {
		(*iface->callbacks)(iface, evt, data);
	}
}

static enum wifi_event get_evt_from_state(enum wifi_state state,
					  enum wifi_state prev)
{
	switch (state) {
	case WIFI_STATE_ASSOCIATED:
		return WIFI_EVT_CONNECTED;
	case WIFI_STATE_DISCONNECTED:
		return WIFI_EVT_DISCONNECTED;
	case WIFI_STATE_INACTIVE:
		if (prev == WIFI_STATE_DISABLED) {
			return WIFI_EVT_STARTED;
		}
		/* fall through */
	default:
		return WIFI_EVT_UNKNOWN;
	}
}

static enum wifi_event get_evt_from_mode(enum wifi_mode mode)
{
	switch (mode) {
	case WIFI_MODE_INFRA:
	case WIFI_MODE_ACCESS_POINT:
	case WIFI_MODE_MESH:
		/* fall through */
	default:
		return WIFI_EVT_UNKNOWN;
	}
}

void wifi_set_state(wifi_iface_t iface, enum wifi_state state)
{
	if (state == iface->state) {
		return;
	}

	iface->state_prev = iface->state;
	iface->state = (uint8_t)state;

	raise_event_with_data(iface, get_evt_from_state(state,
					(enum wifi_state)iface->state_prev), 0);
}

void wifi_set_mode(wifi_iface_t iface, enum wifi_mode mode)
{
	if (mode == iface->mode) {
		return;
	}

	iface->mode_prev = iface->mode;
	iface->mode = (uint8_t)mode;

	raise_event_with_data(iface, get_evt_from_mode(mode), 0);
}

int wifi_register_event_callback(wifi_iface_t iface,
				 const wifi_event_callback_t cb)
{
	iface->callbacks = cb;

	return 0;
}

void wifi_raise_event(wifi_iface_t iface, enum wifi_event evt)
{
	raise_event_with_data(iface, evt, 0);
}

void wifi_raise_event_with_data(wifi_iface_t iface,
		      enum wifi_event evt, const void *data)
{
	raise_event_with_data(iface, evt, data);
}

LIBMCU_WEAK int wifi_connect(wifi_iface_t iface, const struct wifi_conf *param)
{
	if (iface->mode != WIFI_MODE_INFRA) {
		return -ENOTSUP;
	}
	if (iface->state != WIFI_STATE_INACTIVE &&
			iface->state != WIFI_STATE_DISCONNECTED) {
		return -EISCONN;
	}

	unused(param);

	return 0;
}

LIBMCU_WEAK int wifi_disconnect(wifi_iface_t iface)
{
	if (iface->mode != WIFI_MODE_INFRA) {
		return -ENOTSUP;
	}
	if (iface->state != WIFI_STATE_ASSOCIATED) {
		return -ENOTCONN;
	}

	return 0;
}

LIBMCU_WEAK int wifi_scan(wifi_iface_t iface)
{
	unused(iface);
	return 0;
}

LIBMCU_WEAK wifi_iface_t wifi_create(void)
{
	return 0;
}

LIBMCU_WEAK int wifi_get_ap_info(wifi_iface_t iface, struct wifi_ap_info *info)
{
	unused(iface);
	unused(info);
	return 0;
}
