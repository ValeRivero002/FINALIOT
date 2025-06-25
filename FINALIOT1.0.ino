#include "Dispenser.h"
#include "WeightSensor.h"
#include "MQTTManager.h"
#include "CommunicationManager.h"
#include <WiFiManager.h>

WiFiManager wm;  // Instancia global (sin wrapper)

// Declaraciones externas necesarias
extern HX711 scale;
extern String Weight;
extern String PastWeight;
extern bool resetWiFiFlag;
extern const int RESET_WIFI_PIN;
extern WiFiClientSecure wiFiClient;
extern PubSubClient client;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Smart Dispenser...");

  pinMode(RESET_WIFI_PIN, INPUT_PULLUP);

  initDispenser();
  initScale();

  setupWiFi();

  wiFiClient.setCACert(AMAZON_ROOT_CA1);
  wiFiClient.setCertificate(CERTIFICATE);
  wiFiClient.setPrivateKey(PRIVATE_KEY);

  setupMQTT();

  Serial.println("Sistema listo!");
}

void loop() {
  checkWiFiConnection();
  handleResetButton();

  if (resetWiFiFlag) {
    resetWiFiFlag = false;
    resetWiFiSettings();
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static unsigned long lastRead = 0;
  if (millis() - lastRead < 100) return;
  lastRead = millis();

  float g = scale.get_units(3);
  if (g < 3.0f) g = 0.0f;

  Weight = classifyWeight(g);

  Serial.printf("Peso: %.1f g → Estado: %s | WiFi: %s (%d dBm) | MQTT: %s\n", 
                g, Weight.c_str(), WiFi.SSID().c_str(), WiFi.RSSI(),
                client.connected() ? "enviado" : "No enviado");

  if (Weight != PastWeight) {
    Serial.printf("Cambio detectado: '%s' → '%s'\n", PastWeight.c_str(), Weight.c_str());
    reportShadow();
    PastWeight = Weight;
  }
}
