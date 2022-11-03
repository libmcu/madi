Network
=======

MQTT
----
.. uml::
    :caption: A Class Diagram

     enum mqtt_event_type {
    	MQTT_EVT_CONNACK
    	MQTT_EVT_DISCONNECT
    	MQTT_EVT_PUBLISH
    	MQTT_EVT_PUBACK /' Acknowledgment for published message with QoS 1. '/
    	MQTT_EVT_PUBREC /' Reception confirmation for published message with QoS 2. '/
    	MQTT_EVT_PUBREL /' Release of published message with QoS 2. '/
    	MQTT_EVT_PUBCOMP /' Confirmation to a publish release message with QoS 2. '/
    	MQTT_EVT_SUBACK
    	MQTT_EVT_UNSUBACK
    	MQTT_EVT_PINGRESP
    }
    
    struct mqtt_conn_params {
    	client_id
    	username
    	userpass
    	will
    	keepalive
    }
    
    struct mqtt_topic {
    	const uint8_t *pathname
    	size_t pathname_len
    	uint8_t qos
    }
    
    struct mqtt_payload {
    	const uint8_t *data
    	size_t data_len
    }
    
    struct mqtt_message {
    	struct mqtt_topic topic
    	struct mqtt_payload payload
    	uint16_t id
    }
    
    struct mqtt_subscription_list {
    	struct mqtt_topic *list
    	uint16_t count
    	uint16_t id
    }
    
    struct mqtt_event {
    	struct mqtt_message message
    	enum mqtt_event_type type
    }
    
    class mqtt_client {
    	const struct transport_interface *transport
    
    	int mqtt_client_init(struct mqtt_client *client, mqtt_event_callback_t event_callback)
    	int mqtt_connect(struct mqtt_client *client, struct mqtt_conn_params *params)
    	int mqtt_disconnect(struct mqtt_client *client)
    	int mqtt_publish(struct mqtt_client *client, const struct mqtt_message *msg)
    	int mqtt_subscribe(struct mqtt_client *client, const struct mqtt_subscription_list *params)
    	int mqtt_unsubscribe(struct mqtt_client *client, const struct mqtt_subscription_list *params)
    	int mqtt_step(struct mqtt_client *client)
    
    	void (*mqtt_event_callback_t)(struct mqtt_client *client, struct mqtt_event *event)
    }

.. doxygenfile:: include/net/protocols/mqtt.h
   :project: fpl

Transport
---------

.. uml::

    interface transport {
    	write(data, data_len)
    	read(buf, bufsize)
    }

.. doxygenfile:: include/net/transport.h
   :project: fpl
.. doxygenfile:: include/net/transport_interface.h
   :project: fpl

WiFi
----

.. uml::
    :caption: A Class Diagram

    struct WifiInfo {
    	uint8_t ssid[]
    	uint8_t ssid_len
    	uint8_t mac[]
    	uint8_t mac_len
    	int8_t rssi
    	uint8_t band
    	uint8_t channel
    	uint8_t security
    	uint8_t mfp /* management frame protection */
    }
    
    struct WifiConf {
    	uint8_t *ssid
    	uint8_t ssid_len
    	uint8_t *psk
    	uint8_t psk_len
    	uint8_t *sae
    	uint8_t sae_len
    	uint8_t band
    	uint8_t channel
    	uint8_t security
    	uint8_t mfp
    	int timeout_ms
    }
    
    enum WifiEvent {
    	WIFI_EVENT_CONNECT_RESULT
    	WIFI_EVENT_DISCONNECT_RESULT
    	WIFI_EVENT_SCAN_RESULT
    	WIFI_EVENT_SCAN_DONE
    	WIFI_EVENT_AP_ACTIVATE_RESULT
    	WIFI_EVENT_AP_DEACTIVATE_RESULT
    }
    
    enum WifiSecurity {
    	WIFI_SEC_TYPE_PSK
    	WIFI_SEC_TYPE_PSK_SHA256
    	WIFI_SEC_TYPE_PSK_SAE
    }
    
    enum WifiMfp {
    	WIFI_MFP_DISABLED
    	WIFI_MFP_OPTIONAL
    	WIFI_MFP_REQUIRED
    }
    
    enum WifiFreqBand {
    	WIFI_FREQ_BAND_2_4_GHZ
    	WIFI_FREQ_BAND_5_GHZ
    	WIFI_FREQ_BAND_6_GHZ
    }
    
    enum WifiMode {
    	WIFI_MODE_INFRA
    	WIFI_MODE_AP
    	WIFI_MODE_MESH
    }
    
    enum WifiLinkMode {
    	WIFI_LM_80211
    	WIFI_LM_80211_b
    	WIFI_LM_80211_a
    	WIFI_LM_80211_g
    	WIFI_LM_80211_n_HT
    	WIFI_LM_80211_ac_VHT
    	WIFI_LM_80211_ax_HE
    	WIFI_LM_80211_ax_6_GHZ_HE
    	WIFI_LM_80211_BE_EHT
    }
    
    interface Wifi {
    	wifi_connect(handle, struct WifiConf)
    	wifi_disconnect(handle)
    	wifi_scan(handle)
    	wifi_activate_ap(handle)
    	wifi_deactivate_ap(handle)
    	wifi_add_event_callback(handle, cb)
    }

.. doxygenfile:: include/net/wifi.h
   :project: fpl
.. doxygenfile:: include/net/wifi_interface.h
   :project: fpl
