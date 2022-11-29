/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_BLE_H
#define FPL_BLE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "net/ble_interface.h"
#include "net/util.h"
#include <stdbool.h>

#define BLE_ADV_MIN_INTERVAL_MS			20U
#define BLE_ADV_MAX_INTERVAL_MS			10028U

void ble_adv_payload_init(struct ble_adv_payload *buf);
int ble_adv_payload_add(struct ble_adv_payload *buf, uint8_t type,
			const void *data, uint8_t data_len);

static inline int ble_enable(struct ble *iface,
		enum ble_device_addr addr_type, uint8_t addr[BLE_ADDR_LEN])
{
	return ((struct ble_interface *)iface)->enable(iface, addr_type, addr);
}

static inline int ble_disable(struct ble *iface)
{
	return ((struct ble_interface *)iface)->disable(iface);
}

static inline enum ble_device_addr ble_get_device_address(struct ble *iface,
		uint8_t addr[BLE_ADDR_LEN])
{
	return ((struct ble_interface *)iface)->get_device_address(iface, addr);
}

static inline void ble_register_gap_event_callback(struct ble *iface,
						  ble_event_callback_t cb)
{
	((struct ble_interface *)iface)->register_gap_event_callback(iface, cb);
}

static inline void ble_register_gatt_event_callback(struct ble *iface,
						  ble_event_callback_t cb)
{
	((struct ble_interface *)iface)->register_gatt_event_callback(iface, cb);
}

static inline int ble_adv_init(struct ble *iface, enum ble_adv_mode mode)
{
	return ((struct ble_interface *)iface)->adv_init(iface, mode);
}

static inline int ble_adv_set_interval(struct ble *iface,
			uint16_t min_ms, uint16_t max_ms)
{
	return ((struct ble_interface *)iface)->adv_set_interval(iface,
						min_ms, max_ms);
}

static inline int ble_adv_set_duration(struct ble *iface, uint32_t msec)
{
	return ((struct ble_interface *)iface)->adv_set_duration(iface, msec);
}

static inline int ble_adv_set_payload(struct ble *iface,
			const struct ble_adv_payload *payload)
{
	return ((struct ble_interface *)iface)->adv_set_payload(iface, payload);
}

static inline int ble_adv_set_scan_response(struct ble *iface,
			const struct ble_adv_payload *payload)
{
	return ((struct ble_interface *)iface)->adv_set_scan_response(iface,
						payload);
}

static inline int ble_adv_start(struct ble *iface)
{
	return ((struct ble_interface *)iface)->adv_start(iface);
}

static inline int ble_adv_stop(struct ble *iface)
{
	return ((struct ble_interface *)iface)->adv_stop(iface);
}

static inline struct ble_gatt_service *ble_gatt_create_service(
		struct ble *iface, void *mem, uint16_t memsize,
		const uint8_t *uuid, uint8_t uuid_len,
		bool primary, uint8_t nr_chrs)
{
	return ((struct ble_interface *)iface)->gatt_create_service(mem,
			memsize, uuid, uuid_len, primary, nr_chrs);
}

static inline const uint16_t *ble_gatt_add_characteristic(struct ble *iface,
		struct ble_gatt_service *svc,
		const uint8_t *uuid, uint8_t uuid_len,
		struct ble_gatt_characteristic *chr)
{
	return ((struct ble_interface *)iface)->gatt_add_characteristic(svc,
			uuid, uuid_len, chr);
}

static inline int ble_gatt_register_service(struct ble *iface,
		struct ble_gatt_service *svc)
{
	return ((struct ble_interface *)iface)->gatt_register_service(svc);
}

static inline int ble_gatt_response(struct ble *iface,
		struct ble_handler_context *ctx,
		const void *data, uint16_t datasize)
{
	return ((struct ble_interface *)iface)->gatt_response(ctx,
				data, datasize);
}

static inline int ble_gatt_notify(struct ble *iface,
		const void *attr_handle, const void *data, uint16_t datasize)
{
	return ((struct ble_interface *)iface)->gatt_notify(iface, attr_handle,
				data, datasize);
}

#if defined(BLE_DEFAULT_INTERFACE)
#define ble_create_default	CONCAT(BLE_DEFAULT_INTERFACE, _ble_create)
#define ble_destroy_default	CONCAT(BLE_DEFAULT_INTERFACE, _ble_destroy)
#endif

struct ble *ble_create_default(void);
void ble_destroy_default(struct ble *iface);

#if defined(__cplusplus)
}
#endif

#endif /* FPL_BLE_H */
