/*
 * SPDX-FileCopyrightText: 2022 Kyunghwan Kwon <k@mononn.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "cli.h"
#include <stdio.h>
#include <string.h>

#include "net/protocols/mqtt.h"
#include "net/transport.h"

#if !defined(MQTT_TEST_ENDPOINT)
#define MQTT_TEST_ENDPOINT	""
#endif
#if !defined(MQTT_TEST_PORT)
#define MQTT_TEST_PORT		8883
#endif
#if !defined(MQTT_TEST_CLIENT_ID)
#define MQTT_TEST_CLIENT_ID	"hello"
#endif
#if !defined(MQTT_TEST_CA)
#define MQTT_TEST_CA		""
#endif
#if !defined(MQTT_TEST_CERT)
#define MQTT_TEST_CERT		""
#endif
#if !defined(MQTT_TEST_KEY)
#define MQTT_TEST_KEY		""
#endif

#define MQTT_TEST_TIMEOUT_MS	10000
#define MQTT_TEST_BUF_SIZE	1024

static void println(const struct cli_io *io, const char *str)
{
	io->write(str, strlen(str));
	io->write("\n", 1);
}

static void on_mqtt_events(struct mqtt_client *ctx, struct mqtt_event *evt)
{
	(void)ctx;
	(void)evt;
}

static int do_init(struct mqtt_client **mqtt)
{
	static uint8_t buf[MQTT_TEST_BUF_SIZE];
	struct mqtt_conn_param mqtt_conf = {
		.client_id = {
			.value = (const uint8_t *)MQTT_TEST_CLIENT_ID,
			.length = sizeof(MQTT_TEST_CLIENT_ID),
		},
		.timeout_ms = MQTT_TEST_TIMEOUT_MS,
		.keepalive_sec = 60,
		.clean_session = true,
	};

	struct transport_conn_param transport_conf = {
		.endpoint = MQTT_TEST_ENDPOINT,
		.endpoint_len = sizeof(MQTT_TEST_ENDPOINT),
		.port = MQTT_TEST_PORT,
		.timeout_ms = MQTT_TEST_TIMEOUT_MS,
	};
	transport_set_ca_cert(&transport_conf,
			MQTT_TEST_CA, sizeof(MQTT_TEST_CA));
	transport_set_client_cert(&transport_conf,
			MQTT_TEST_CERT, sizeof(MQTT_TEST_CERT));
	transport_set_client_key(&transport_conf,
			MQTT_TEST_KEY, sizeof(MQTT_TEST_KEY));
	// transport_set_endpoint(v, len, port)

	struct transport *tls = transport_create_default(&transport_conf);

	*mqtt = mqtt_client_create();
	mqtt_set_transport(*mqtt, tls);
	mqtt_set_rx_buffer(*mqtt, buf, sizeof(buf));
	mqtt_set_conn_param(*mqtt, &mqtt_conf);

	return mqtt_client_init(*mqtt, on_mqtt_events);
}

static int do_publish(struct mqtt_client *mqtt,
		const char *topic, const char *value)
{
	struct mqtt_message msg = {
		.topic = {
			.pathname = topic,
			.pathname_len = strlen(topic),
			.qos = 0,
		},
		.payload = {
			.value = (const uint8_t *)value,
			.length = strlen(value),
		},
	};

	return mqtt_publish(mqtt, &msg, false);
}

static int do_subscribe(struct mqtt_client *mqtt, const char *topic)
{
	struct mqtt_topic msg = {
		.pathname = topic,
		.pathname_len = strlen(topic),
		.qos = 0,
	};

	return mqtt_subscribe(mqtt, &msg, 1);
}

static int do_unsubscribe(struct mqtt_client *mqtt, const char *topic)
{
	struct mqtt_topic msg = {
		.pathname = topic,
		.pathname_len = strlen(topic),
		.qos = 0,
	};

	return mqtt_unsubscribe(mqtt, &msg, 1);
}

static int do_listen(struct mqtt_client *mqtt, const struct cli_io *io)
{
	(void)io;

	int rc = 0;
#if defined(CLI_ASYNC)
	char ch = 0;

	io->write("type q to quit\n", 15);
#endif

	do {
		if ((rc = mqtt_step(mqtt))) {
			break;
		}
#if defined(CLI_ASYNC)
	} while (io->read(&ch, 1) != 1 || ch != 'q');
#else
	} while (1);
#endif

	return rc;
}

static void process(int argc, const char *argv[], const struct cli_io *io)
{
	static struct mqtt_client *mqtt;
	int rc = 0;

	if (argc < 2) {
		return;
	}

	if (strcmp(argv[1], "help") == 0) {
		println(io, "subcommands:\n\n\tinit\n\tconnect\n\tdisconnect" \
			"\n\tpublish\n\tsubscribe");
	} else if (strcmp(argv[1], "init") == 0) {
		rc = do_init(&mqtt);
	} else if (strcmp(argv[1], "connect") == 0) {
		rc = mqtt_connect(mqtt);
	} else if (strcmp(argv[1], "disconnect") == 0) {
		rc = mqtt_disconnect(mqtt);
	} else if (strcmp(argv[1], "step") == 0) {
		rc = mqtt_step(mqtt);
	} else if (strcmp(argv[1], "listen") == 0) {
		rc = do_listen(mqtt, io);
	} else if (strcmp(argv[1], "publish") == 0 && argc == 4) {
		rc = do_publish(mqtt, argv[2], argv[3]);
	} else if (strcmp(argv[1], "subscribe") == 0 && argc == 3) {
		rc = do_subscribe(mqtt, argv[2]);
	} else if (strcmp(argv[1], "unsubscribe") == 0 && argc == 3) {
		rc = do_unsubscribe(mqtt, argv[2]);
	}

	char buf[16];
	snprintf(buf, sizeof(buf)-1, "RC: %d\n", rc);
	io->write(buf, strlen(buf));
}

cli_cmd_error_t cli_cmd_mqtt(int argc, const char *argv[], const void *env)
{
	struct cli const *cli = (struct cli const *)env;

	process(argc, argv, cli->io);

	return CLI_CMD_SUCCESS;
}
