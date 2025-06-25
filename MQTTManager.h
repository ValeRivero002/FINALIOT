#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern WiFiClientSecure wiFiClient;
extern PubSubClient client;

extern const char* MQTT_BROKER;
extern const int MQTT_PORT;
extern const char* CLIENT_ID;

void setupMQTT();

#endif
