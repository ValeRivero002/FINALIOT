#include "MQTTManager.h"
#include "CommunicationManager.h"

const char* MQTT_BROKER = "a2dbaocrje4klj-ats.iot.us-east-2.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP32-SMART-DISPENSER-001";

WiFiClientSecure wiFiClient;
PubSubClient client(wiFiClient);

void setupMQTT() {
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
}
