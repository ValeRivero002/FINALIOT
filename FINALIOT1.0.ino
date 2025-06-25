#include "Dispenser.h"
#include "WeightSensor.h"
#include "MQTTManager.h"
#include "CommunicationManager.h"
#include "certificates.h"

#include <WiFiManager.h>
WiFiManager wm;

extern HX711 scale;
extern String Weight;
extern String PastWeight;
extern WiFiClientSecure wiFiClient;
extern PubSubClient client;

const int RESET_WIFI_PIN = 2;
bool resetWiFiFlag = false;
const char* CLIENT_ID = "ESP32-SMART-DISPENSER-001";
void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Smart Dispenser...");

  pinMode(RESET_WIFI_PIN, INPUT_PULLUP);

  initDispenser();
  initScale();

  // Configurar WiFiManager directamente
  wm.setConfigPortalTimeout(180);  // 3 minutos
  bool connected = wm.autoConnect("SmartDispenser_Setup", "12345678");
  if (!connected) {
    Serial.println("❌ No se pudo conectar a WiFi. Reiniciando...");
    delay(1000);
    ESP.restart();
  }
  Serial.println("✅ WiFi conectado!");
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  // Configurar TLS
  wiFiClient.setCACert(AMAZON_ROOT_CA1);
  wiFiClient.setCertificate(CERTIFICATE);
  wiFiClient.setPrivateKey(PRIVATE_KEY);

  setupMQTT();

  Serial.println("Sistema listo!");
}

void loop() {
  // Verificar botón de reset WiFi
  static bool buttonPressed = false;
  static unsigned long lastPress = 0;
  if (digitalRead(RESET_WIFI_PIN) == LOW) {
    if (!buttonPressed) {
      lastPress = millis();
      buttonPressed = true;
    }
  } else {
    if (buttonPressed) {
      buttonPressed = false;
      unsigned long pressTime = millis() - lastPress;
      if (pressTime >= 3000) {
        Serial.println("🔁 Presión larga → reseteando WiFi...");
        wm.resetSettings();
        delay(1000);
        ESP.restart();
      } else {
        Serial.println("ℹ️ Presión corta → info de WiFi:");
        Serial.printf("SSID: %s, IP: %s, RSSI: %d dBm\n", 
                      WiFi.SSID().c_str(), 
                      WiFi.localIP().toString().c_str(), 
                      WiFi.RSSI());
      }
    }
  }

  // Verificar estado de conexión WiFi cada 30s
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 30000) {
    lastCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi desconectado. Intentando reconectar...");
      WiFi.reconnect();
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
      }
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n⚠️ No se pudo reconectar. Reiniciando...");
        ESP.restart();
      } else {
        Serial.println("\n✅ WiFi reconectado!");
      }
    }
  }

  // Reconectar MQTT si se pierde conexión
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lectura y publicación del peso
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

