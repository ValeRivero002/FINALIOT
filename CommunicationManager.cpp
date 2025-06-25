#include "CommunicationManager.h"
#include "ServoActuator.h"
#include "certificates.h"

StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;
StaticJsonDocument<JSON_OBJECT_SIZE(64)> outputDoc;
char outputBuffer[256];

void reportShadow() {
  outputDoc.clear();
  auto s = outputDoc.createNestedObject("state").createNestedObject("reported");
  s["weight"] = Weight;
  s["dispenser_state"] = Dispenser_state;
  serializeJson(outputDoc, outputBuffer);
  client.publish(UPDATE_TOPIC, outputBuffer);
}

void applyDispenserState() {
  if (Dispenser_state == "open") {
    myServo.write(0);
  } else {
    myServo.write(90);
  }
  reportShadow();
}

void callback(char* topic, byte* payload, unsigned int length) {
  if (String(topic) != UPDATE_DELTA_TOPIC) return;

  DeserializationError err = deserializeJson(inputDoc, payload);
  if (err) return;

  JsonObject state = inputDoc["state"].as<JsonObject>();
  if (state.containsKey("dispenser_state")) {
    String newState = state["dispenser_state"].as<String>();
    if (newState != Dispenser_state) {
      Dispenser_state = newState;
      applyDispenserState();
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(CLIENT_ID)) {
      client.subscribe(UPDATE_DELTA_TOPIC);
      reportShadow();
    } else {
      delay(5000);
    }
  }
}
