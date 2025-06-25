#ifndef COMMUNICATION_MANAGER_H
#define COMMUNICATION_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void reportShadow();
void applyDispenserState();

extern WiFiClientSecure wiFiClient;
extern PubSubClient client;
extern const char* CLIENT_ID;
extern const char* UPDATE_TOPIC;
extern const char* UPDATE_DELTA_TOPIC;

extern String Dispenser_state;
extern String Weight;

#endif
