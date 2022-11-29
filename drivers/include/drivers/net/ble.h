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

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#if !defined(BLE_ADV_MAX_PAYLOAD_SIZE)
#define BLE_ADV_MAX_PAYLOAD_SIZE		\
		(37U - 6U/*advertiser address*/ - 2U/*header+length*/)
#endif
#define BLE_TIME_FOREVER			UINT32_MAX
#define BLE_ADDR_LEN				6U
#define BLE_ADV_MIN_INTERVAL_MS			20U
#define BLE_ADV_MAX_INTERVAL_MS			10028U

enum ble_adv_mode {
	BLE_ADV_IND,         /**< connectable     scannable     undirected */
	BLE_ADV_DIRECT_IND,  /**< connectable     scannable     directed */
	BLE_ADV_NONCONN_IND, /**< non-connectable non-scannable undirected */
	BLE_ADV_SCAN_IND,    /**< non-connectable scannable     undirected */
};

enum ble_gap_evt {
	BLE_GAP_EVT_UNKNOWN,
	BLE_GAP_EVT_READY,
	BLE_GAP_EVT_CONNECTED,
	BLE_GAP_EVT_DISCONNECTED,
	BLE_GAP_EVT_ADV_COMPLETE,
	BLE_GAP_EVT_ADV_SUSPENDED,
	BLE_GAP_EVT_MTU,
	BLE_GAP_EVT_MAX,
};

enum ble_gatt_evt {
	BLE_GATT_EVT_READ_CHR,
	BLE_GATT_EVT_WRITE_CHR,
	BLE_GATT_EVT_READ_DSC,
	BLE_GATT_EVT_WRITE_DSC,
};

enum ble_device_addr {
	BLE_ADDR_PUBLIC,
	BLE_ADDR_STATIC_RPA,
	BLE_ADDR_PRIVATE_RPA,
	BLE_ADDR_PRIVATE_NRPA,
};

enum ble_gatt_op {
	BLE_GATT_OP_READ			= 0x0001,
	BLE_GATT_OP_WRITE			= 0x0002,
	BLE_GATT_OP_NOTIFY			= 0x0004,
	BLE_GATT_OP_INDICATE			= 0x0008,
	BLE_GATT_OP_ENC_READ			= 0x0010,
	BLE_GATT_OP_AUTH_READ			= 0x0020,
	BLE_GATT_OP_AUTHORIZE_READ		= 0x0040,
	BLE_GATT_OP_ENC_WRITE			= 0x0080,
	BLE_GATT_OP_AUTH_WRITE			= 0x0100,
	BLE_GATT_OP_AUTHORIZE_WRITE		= 0x0200,
};

struct ble;
struct ble_gatt_service;

typedef void (*ble_event_callback_t)(struct ble *self,
		uint8_t evt, const void *msg);

struct ble_handler_context {
	uint8_t event_type;
	void *ctx;
};

typedef void (*ble_gatt_characteristic_handler)(struct ble_handler_context *ctx,
		const void *data, uint16_t datasize, void *user_ctx);

struct ble_adv_payload {
	uint8_t payload[BLE_ADV_MAX_PAYLOAD_SIZE];
	uint8_t index;
};

struct ble_gatt_characteristic {
	ble_gatt_characteristic_handler handler;
	void *user_ctx;
	uint16_t op;
};

struct ble_api {
	int (*enable)(struct ble *self, enum ble_device_addr addr_type,
			uint8_t addr[BLE_ADDR_LEN]);
	int (*disable)(struct ble *self);
	enum ble_device_addr (*get_device_address)(struct ble *self,
			uint8_t addr[BLE_ADDR_LEN]);

	void (*register_gap_event_callback)(struct ble *self,
			ble_event_callback_t cb);
	void (*register_gatt_event_callback)(struct ble *self,
			ble_event_callback_t cb);

	int (*adv_init)(struct ble *self, enum ble_adv_mode mode);
	int (*adv_set_interval)(struct ble *self,
			uint16_t min_ms, uint16_t max_ms);
	int (*adv_set_duration)(struct ble *self, uint32_t msec);
	int (*adv_set_payload)(struct ble *self,
			const struct ble_adv_payload *payload);
	int (*adv_set_scan_response)(struct ble *self,
			const struct ble_adv_payload *payload);
	int (*adv_start)(struct ble *self);
	int (*adv_stop)(struct ble *self);

	struct ble_gatt_service *(*gatt_create_service)(void *mem, uint16_t memsize,
			const uint8_t *uuid, uint8_t uuid_len,
			bool primary, uint8_t nr_characteristics);
	const uint16_t *(*gatt_add_characteristic)(struct ble_gatt_service *svc,
			const uint8_t *uuid, uint8_t uuid_len,
			struct ble_gatt_characteristic *chr);
	int (*gatt_register_service)(struct ble_gatt_service *svc);
	int (*gatt_response)(struct ble_handler_context *ctx,
			const void *data, uint16_t datasize);
	int (*gatt_notify)(struct ble *self, const void *attr_handle,
			const void *data, uint16_t datasize);
};

#include "drivers/net/ble_interface.h"

#if defined(__cplusplus)
}
#endif

#endif /* FPL_BLE_H */
