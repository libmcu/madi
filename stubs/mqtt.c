#include "net/protocols/mqtt.h"
#include "libmcu/compiler.h"

LIBMCU_WEAK
struct mqtt_client *mqtt_client_create(void)
{
	return NULL;
}

LIBMCU_WEAK
void mqtt_client_delete(struct mqtt_client *client)
{
	unused(client);
}

LIBMCU_WEAK
int mqtt_client_init(struct mqtt_client *client, mqtt_event_callback_t cb)
{
	unused(client);
	unused(cb);
	return 0;
}

LIBMCU_WEAK
int mqtt_connect(struct mqtt_client *client)
{
	unused(client);
	return 0;
}

LIBMCU_WEAK
int mqtt_disconnect(struct mqtt_client *client)
{
	unused(client);
	return 0;
}

LIBMCU_WEAK
int mqtt_publish(struct mqtt_client *client, const struct mqtt_message *msg,
		 bool retain)
{
	unused(client);
	unused(msg);
	unused(retain);
	return 0;
}

LIBMCU_WEAK
int mqtt_subscribe(struct mqtt_client *client,
		const struct mqtt_topic *topics, uint16_t count)
{
	unused(client);
	unused(topics);
	unused(count);
	return 0;
}

LIBMCU_WEAK
int mqtt_unsubscribe(struct mqtt_client *client,
		const struct mqtt_topic *topics, uint16_t count)
{
	unused(client);
	unused(topics);
	unused(count);
	return 0;
}

LIBMCU_WEAK
int mqtt_step(struct mqtt_client *client)
{
	unused(client);
	return 0;
}
