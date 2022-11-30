/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_BLE_INTERFACE_H
#define FPL_BLE_INTERFACE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "drivers/net/ble.h"

void ble_adv_payload_init(struct ble_adv_payload *buf);
int ble_adv_payload_add(struct ble_adv_payload *buf, uint8_t type,
			const void *data, uint8_t data_len);

static inline int ble_enable(struct ble *self,
		enum ble_device_addr addr_type, uint8_t addr[BLE_ADDR_LEN])
{
	return ((struct ble_api *)self)->enable(self, addr_type, addr);
}

static inline int ble_disable(struct ble *self)
{
	return ((struct ble_api *)self)->disable(self);
}

static inline enum ble_device_addr ble_get_device_address(struct ble *self,
		uint8_t addr[BLE_ADDR_LEN])
{
	return ((struct ble_api *)self)->get_device_address(self, addr);
}

static inline void ble_register_gap_event_callback(struct ble *self,
						  ble_event_callback_t cb)
{
	((struct ble_api *)self)->register_gap_event_callback(self, cb);
}

static inline void ble_register_gatt_event_callback(struct ble *self,
						  ble_event_callback_t cb)
{
	((struct ble_api *)self)->register_gatt_event_callback(self, cb);
}

static inline int ble_adv_init(struct ble *self, enum ble_adv_mode mode)
{
	return ((struct ble_api *)self)->adv_init(self, mode);
}

static inline int ble_adv_set_interval(struct ble *self,
			uint16_t min_ms, uint16_t max_ms)
{
	return ((struct ble_api *)self)->adv_set_interval(self,
						min_ms, max_ms);
}

static inline int ble_adv_set_duration(struct ble *self, uint32_t msec)
{
	return ((struct ble_api *)self)->adv_set_duration(self, msec);
}

static inline int ble_adv_set_payload(struct ble *self,
			const struct ble_adv_payload *payload)
{
	return ((struct ble_api *)self)->adv_set_payload(self, payload);
}

static inline int ble_adv_set_scan_response(struct ble *self,
			const struct ble_adv_payload *payload)
{
	return ((struct ble_api *)self)->adv_set_scan_response(self,
						payload);
}

static inline int ble_adv_start(struct ble *self)
{
	return ((struct ble_api *)self)->adv_start(self);
}

static inline int ble_adv_stop(struct ble *self)
{
	return ((struct ble_api *)self)->adv_stop(self);
}

static inline struct ble_gatt_service *ble_gatt_create_service(
		struct ble *self, void *mem, uint16_t memsize,
		const uint8_t *uuid, uint8_t uuid_len,
		bool primary, uint8_t nr_chrs)
{
	return ((struct ble_api *)self)->gatt_create_service(mem,
			memsize, uuid, uuid_len, primary, nr_chrs);
}

static inline const uint16_t *ble_gatt_add_characteristic(struct ble *self,
		struct ble_gatt_service *svc,
		const uint8_t *uuid, uint8_t uuid_len,
		struct ble_gatt_characteristic *chr)
{
	return ((struct ble_api *)self)->gatt_add_characteristic(svc,
			uuid, uuid_len, chr);
}

static inline int ble_gatt_register_service(struct ble *self,
		struct ble_gatt_service *svc)
{
	return ((struct ble_api *)self)->gatt_register_service(svc);
}

static inline int ble_gatt_response(struct ble *self,
		struct ble_handler_context *ctx,
		const void *data, uint16_t datasize)
{
	return ((struct ble_api *)self)->gatt_response(ctx,
				data, datasize);
}

static inline int ble_gatt_notify(struct ble *self,
		const void *attr_handle, const void *data, uint16_t datasize)
{
	return ((struct ble_api *)self)->gatt_notify(self, attr_handle,
				data, datasize);
}

#if defined(__cplusplus)
}
#endif

#endif /* FPL_BLE_INTERFACE_H */
