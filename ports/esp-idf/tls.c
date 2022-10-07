/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "net/transport.h"
#include "esp_tls.h"

enum tls_state {
	TLS_STATE_UNKNOWN,
	TLS_STATE_CONNECTING,
	TLS_STATE_CONNECTED,
	TLS_STATE_DISCONNECTED,
};

struct tls_transport {
	struct transport_interface base;
	struct transport_conn_param param;
	enum tls_state state;
	esp_tls_t *ctx;
};

static bool is_connected(struct tls_transport *tls)
{
	return tls->state == TLS_STATE_CONNECTED;
}

static bool is_disconnected(struct tls_transport *tls)
{
	return tls->state == TLS_STATE_DISCONNECTED;
}

static void disconnect_internal(struct tls_transport *tls)
{
	tls->state = TLS_STATE_DISCONNECTED;
	esp_tls_conn_destroy(tls->ctx);

	tls->ctx = NULL;
}

static int connect_internal(struct tls_transport *tls)
{
	int rc = -ENOMEM;

	tls->state = TLS_STATE_CONNECTING;

	if ((tls->ctx = esp_tls_init()) == NULL) {
		goto out_err;
	}

	const esp_tls_cfg_t conf = {
		.cacert_buf = tls->param.ca_cert,
		.cacert_bytes = tls->param.ca_cert_len,
		.clientcert_buf = tls->param.client_cert,
		.clientcert_bytes = tls->param.client_cert_len,
		.clientkey_buf = tls->param.client_key,
		.clientkey_bytes = tls->param.client_key_len,
		.timeout_ms = tls->param.timeout_ms,
		.non_block = true,
	};

	rc = (int)esp_tls_conn_new_sync(tls->param.endpoint,
			tls->param.endpoint_len, tls->param.port, &conf, tls->ctx);

	if (rc == 1) {
		tls->state = TLS_STATE_CONNECTED;
		return 0;
	}

out_err:
	disconnect_internal(tls);
	return rc;
}

static int write_impl(struct transport_interface *self,
		const void *data, size_t data_len)
{
	struct tls_transport *iface = (struct tls_transport *)self;
	const uint8_t *p = (const uint8_t *)data;
	int bytes_sent = 0;
	int rc = 0;

	if (!is_connected(iface)) {
		return -ENOTCONN;
	}

	while (bytes_sent < data_len) {
		rc = (int)esp_tls_conn_write(iface->ctx, &p[bytes_sent],
			       data_len - bytes_sent);

		if (rc == ESP_TLS_ERR_SSL_WANT_READ ||
				rc == ESP_TLS_ERR_SSL_WANT_WRITE) {
			rc = 0;
		} else if (rc < 0) {
			break;
		}

		bytes_sent += rc;
	}

	return rc;
}

static int read_impl(struct transport_interface *self,
		void *buf, size_t bufsize)
{
	struct tls_transport *p = (struct tls_transport *)self;

	if (!is_connected(p)) {
		return -ENOTCONN;
	}

	int rc = (int)esp_tls_conn_read(p->ctx, buf, bufsize);

	if (rc == ESP_TLS_ERR_SSL_WANT_READ ||
				rc == ESP_TLS_ERR_SSL_WANT_WRITE) {
		rc = 0;
	}

	return rc;
}

static int connect_impl(struct transport_interface *self)
{
	struct tls_transport *iface = (struct tls_transport *)self;

	if (is_connected(iface)) {
		return -EISCONN;
	}

	return connect_internal(iface);
}

static int disconnect_impl(struct transport_interface *self)
{
	struct tls_transport *iface = (struct tls_transport *)self;

	if (is_disconnected(iface)) {
		return -ENOTCONN;
	}

	disconnect_internal(iface);

	return 0;
}

struct transport_interface *tls_transport_create(
		const struct transport_conn_param *param)
{
	struct tls_transport *iface =
		(struct tls_transport *)calloc(1, sizeof(*iface));

	if (iface == NULL) {
		return NULL;
	}

	memcpy(&iface->param, param, sizeof(*param));

	iface->base.write = write_impl;
	iface->base.read = read_impl;
	iface->base.connect = connect_impl;
	iface->base.disconnect = disconnect_impl;

	return &iface->base;
}

void tls_transport_delete(struct transport_interface *instance)
{
	struct tls_transport *iface = (struct tls_transport *)instance;
	free(iface);
}
