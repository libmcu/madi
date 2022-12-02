/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FPL_MQTT_H
#define FPL_MQTT_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

enum mqtt_event_type {
	MQTT_EVT_UNKNOWN,
	MQTT_EVT_CONNACK,
	MQTT_EVT_DISCONNECT,
	MQTT_EVT_PUBLISH,
	MQTT_EVT_PUBACK,
	MQTT_EVT_PUBREC,
	MQTT_EVT_PUBREL,
	MQTT_EVT_PUBCOMP,
	MQTT_EVT_SUBACK,
	MQTT_EVT_UNSUBACK,
	MQTT_EVT_PINGRESP,
};

enum mqtt_event_status {
	MQTT_EVT_STATUS_UNKNOWN,
	MQTT_EVT_STATUS_SUBACK_QOS0,
	MQTT_EVT_STATUS_SUBACK_QOS1,
	MQTT_EVT_STATUS_SUBACK_QOS2,
	MQTT_EVT_STATUS_SUBACK_FAIL,
};

struct mqtt_topic {
	const char *pathname;
	size_t pathname_len;
	uint8_t qos;
};

struct mqtt_vlen {
	const uint8_t *value;
	size_t length;
};

struct mqtt_message {
	struct mqtt_topic topic;
	struct mqtt_vlen payload;
};

struct mqtt_conn_param {
	struct mqtt_vlen client_id;
	struct mqtt_vlen *username;
	struct mqtt_vlen *userpass;
	struct mqtt_message *will;
	uint16_t keepalive_sec;
	uint16_t timeout_ms;
	bool clean_session;
};

struct mqtt_event {
	struct mqtt_message message;
	enum mqtt_event_type type;
	enum mqtt_event_status status;
};

struct mqtt_client {
	struct mqtt_conn_param conn_param;
	void *transport;
	uint8_t *rx_buf;
	size_t rx_buf_size;
	uint8_t *tx_buf;
	size_t tx_buf_size;
};

typedef void (*mqtt_event_callback_t)(struct mqtt_client *client,
		struct mqtt_event *event);

struct mqtt_client *mqtt_client_create(void);
void mqtt_client_delete(struct mqtt_client *client);
int mqtt_client_init(struct mqtt_client *client, mqtt_event_callback_t cb);
int mqtt_connect(struct mqtt_client *client);
int mqtt_disconnect(struct mqtt_client *client);
int mqtt_publish(struct mqtt_client *client, const struct mqtt_message *msg,
		 bool retain);
int mqtt_subscribe(struct mqtt_client *client,
		const struct mqtt_topic *topics, uint16_t count);
int mqtt_unsubscribe(struct mqtt_client *client,
		const struct mqtt_topic *topics, uint16_t count);
int mqtt_step(struct mqtt_client *client);

#define mqtt_set_transport(p_mqtt, p_transport)		\
		((p_mqtt)->transport = (p_transport))
#define mqtt_set_rx_buffer(p_mqtt, p_buf, l)		\
		((p_mqtt)->rx_buf = (p_buf), (p_mqtt)->rx_buf_size = (l))
#define mqtt_set_conn_param(p_mqtt, p_conf)		\
		((p_mqtt)->conn_param = *(p_conf))

#if defined(__cplusplus)
}
#endif

#endif /* FPL_MQTT_H */
