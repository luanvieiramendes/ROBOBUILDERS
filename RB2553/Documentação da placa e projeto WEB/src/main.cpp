#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include "web_page.h"

// ==========================================
// CONFIGURAÇÃO DO PONTO DE ACESSO WI-FI
// ==========================================
const char* AP_SSID = "ESP32-RELAY-30A-8CH";
const byte DNS_PORT = 53;

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

DNSServer dnsServer;
WebServer server(80);

// ==========================================
// MAPEAMENTO DAS GPIOS (ESP-32_RELAY_30A_X8_V1.2)
// ==========================================
enum PinModeType {
  PIN_MODE_OUTPUT = 0,
  PIN_MODE_INPUT  = 1
};

struct GPIOPin {
  uint8_t pin;
  PinModeType mode;
  uint8_t state;
  uint32_t pulseEnd;
};

// Lista de pinos mapeados
GPIOPin pins[] = {
  // --- 8 Canais de Relé de Potência 30A ---
  { 32, PIN_MODE_OUTPUT, 0, 0 }, // Relé 1 (Canal 1 - 30A)
  { 33, PIN_MODE_OUTPUT, 0, 0 }, // Relé 2 (Canal 2 - 30A)
  { 25, PIN_MODE_OUTPUT, 0, 0 }, // Relé 3 (Canal 3 - 30A)
  { 26, PIN_MODE_OUTPUT, 0, 0 }, // Relé 4 (Canal 4 - 30A)
  { 27, PIN_MODE_OUTPUT, 0, 0 }, // Relé 5 (Canal 5 - 30A)
  { 14, PIN_MODE_OUTPUT, 0, 0 }, // Relé 6 (Canal 6 - 30A)
  { 12, PIN_MODE_OUTPUT, 0, 0 }, // Relé 7 (Canal 7 - 30A)
  { 13, PIN_MODE_OUTPUT, 0, 0 }, // Relé 8 (Canal 8 - 30A)

  // --- Recursos Onboard ---
  { 5,  PIN_MODE_OUTPUT, 0, 0 }, // LED Onboard de Status (GPIO 5)
  { 0,  PIN_MODE_INPUT,  1, 0 }, // Botão Boot / IO0 (GPIO 0 com Pull-Up)

  // --- Barramento de GPIOs / Expansão Digital ---
  { 2,  PIN_MODE_OUTPUT, 0, 0 },
  { 4,  PIN_MODE_OUTPUT, 0, 0 },
  { 15, PIN_MODE_OUTPUT, 0, 0 },
  { 16, PIN_MODE_OUTPUT, 0, 0 },
  { 17, PIN_MODE_OUTPUT, 0, 0 },
  { 18, PIN_MODE_OUTPUT, 0, 0 },
  { 19, PIN_MODE_OUTPUT, 0, 0 },
  { 21, PIN_MODE_OUTPUT, 0, 0 },
  { 22, PIN_MODE_OUTPUT, 0, 0 },
  { 23, PIN_MODE_OUTPUT, 0, 0 },

  // --- Pinos Apenas Entrada (Sensores) ---
  { 34, PIN_MODE_INPUT,  0, 0 },
  { 35, PIN_MODE_INPUT,  0, 0 }
};
const size_t PIN_COUNT = sizeof(pins) / sizeof(pins[0]);

// Lista rápida dos 8 pinos de relé
const uint8_t RELAY_PINS[8] = { 32, 33, 25, 26, 27, 14, 12, 13 };

// ==========================================
// DECLARAÇÃO DE FUNÇÕES
// ==========================================
void setupGPIOs();
void updateReadings();
void checkPulses();
void handleRoot();
void handleStatus();
void handleToggle();
void handleMode();
void handlePulse();
void handleRelays();
void handleAll();
void handleNotFound();

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n==========================================");
  Serial.println("  ESP-32_RELAY_30A_X8_V1.2 - FIRMWARE     ");
  Serial.println("  8 Relés de Potência 30A (CH1 a CH8)     ");
  Serial.println("  Relés: GPIO 32, 33, 25, 26, 27, 14, 12, 13");
  Serial.println("  LED Onboard: GPIO 5                     ");
  Serial.println("  Botão Boot:  GPIO 0 (IO0)               ");
  Serial.println("==========================================");

  // 1. Inicializar todas as GPIOs
  setupGPIOs();

  // 2. Configurar SoftAP Wi-Fi
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(AP_SSID, "");

  Serial.printf("[Wi-Fi] Rede Aberta: %s\n", AP_SSID);
  Serial.printf("[Wi-Fi] IP de Acesso: %s\n", WiFi.softAPIP().toString().c_str());

  // 3. DNS Captivo
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  // 4. Rotas Web REST
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/toggle", HTTP_GET, handleToggle);
  server.on("/api/mode", HTTP_GET, handleMode);
  server.on("/api/pulse", HTTP_GET, handlePulse);
  server.on("/api/relays", HTTP_GET, handleRelays);
  server.on("/api/all", HTTP_GET, handleAll);

  // Captive Portal Handlers
  server.on("/generate_204", HTTP_GET, handleRoot);
  server.on("/gen_204", HTTP_GET, handleRoot);
  server.on("/hotspot-detect.html", HTTP_GET, handleRoot);
  server.on("/ncsi.txt", HTTP_GET, handleRoot);
  server.on("/connecttest.txt", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("[HTTP] Servidor Web Pronto.");
}

// ==========================================
// LOOP
// ==========================================
void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  updateReadings();
  checkPulses();
}

// ==========================================
// CONTROLE DE HARDWARE
// ==========================================
void setupGPIOs() {
  for (size_t i = 0; i < PIN_COUNT; i++) {
    uint8_t p = pins[i].pin;
    if (pins[i].mode == PIN_MODE_OUTPUT) {
      pinMode(p, OUTPUT);
      digitalWrite(p, pins[i].state ? HIGH : LOW);
    } else {
      pinMode(p, INPUT_PULLUP);
      pins[i].state = digitalRead(p);
    }
  }
}

void updateReadings() {
  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (pins[i].mode == PIN_MODE_INPUT) {
      pins[i].state = digitalRead(pins[i].pin);
    }
  }
}

void checkPulses() {
  uint32_t now = millis();
  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (pins[i].pulseEnd > 0 && now >= pins[i].pulseEnd) {
      pins[i].pulseEnd = 0;
      pins[i].state = 0;
      digitalWrite(pins[i].pin, LOW);
    }
  }
}

// ==========================================
// HANDLERS HTTP
// ==========================================
void handleRoot() {
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleStatus() {
  StaticJsonDocument<2048> doc;
  doc["uptime"] = millis() / 1000;

  JsonArray pinArr = doc.createNestedArray("pins");
  for (size_t i = 0; i < PIN_COUNT; i++) {
    JsonObject p = pinArr.createNestedObject();
    p["pin"] = pins[i].pin;
    p["mode"] = (int)pins[i].mode;
    p["val"] = pins[i].state;
  }

  String output;
  serializeJson(doc, output);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", output);
}

void handleToggle() {
  if (!server.hasArg("pin")) {
    server.send(400, "application/json", "{\"error\":\"No pin\"}");
    return;
  }
  uint8_t pinNum = server.arg("pin").toInt();

  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (pins[i].pin == pinNum) {
      if (pins[i].mode == PIN_MODE_OUTPUT) {
        pins[i].state = !pins[i].state;
        pins[i].pulseEnd = 0;
        digitalWrite(pinNum, pins[i].state ? HIGH : LOW);

        char res[64];
        snprintf(res, sizeof(res), "{\"pin\":%d,\"val\":%d}", pinNum, pins[i].state);
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", res);
        return;
      } else {
        server.send(400, "application/json", "{\"error\":\"Pin is set to INPUT\"}");
        return;
      }
    }
  }
  server.send(404, "application/json", "{\"error\":\"Pin not found\"}");
}

void handleMode() {
  if (!server.hasArg("pin") || !server.hasArg("mode")) {
    server.send(400, "application/json", "{\"error\":\"Missing parameters\"}");
    return;
  }
  uint8_t pinNum = server.arg("pin").toInt();
  uint8_t modeVal = server.arg("mode").toInt(); // 0 = Output, 1 = Input

  // Pinos 34 e 35 não podem ser output no ESP32
  if ((pinNum == 34 || pinNum == 35) && modeVal == 0) {
    server.send(400, "application/json", "{\"error\":\"GPIO 34 and 35 are input-only\"}");
    return;
  }

  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (pins[i].pin == pinNum) {
      pins[i].mode = (PinModeType)modeVal;
      pins[i].pulseEnd = 0;
      if (pins[i].mode == PIN_MODE_OUTPUT) {
        pinMode(pinNum, OUTPUT);
        digitalWrite(pinNum, LOW);
        pins[i].state = 0;
      } else {
        pinMode(pinNum, INPUT_PULLUP);
        pins[i].state = digitalRead(pinNum);
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "application/json", "{\"success\":true}");
      return;
    }
  }
  server.send(404, "application/json", "{\"error\":\"Pin not found\"}");
}

void handlePulse() {
  if (!server.hasArg("pin") || !server.hasArg("ms")) {
    server.send(400, "application/json", "{\"error\":\"Missing args\"}");
    return;
  }
  uint8_t pinNum = server.arg("pin").toInt();
  uint32_t duration = constrain(server.arg("ms").toInt(), 50, 10000);

  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (pins[i].pin == pinNum && pins[i].mode == PIN_MODE_OUTPUT) {
      pins[i].state = 1;
      pins[i].pulseEnd = millis() + duration;
      digitalWrite(pinNum, HIGH);

      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "application/json", "{\"success\":true}");
      return;
    }
  }
  server.send(404, "application/json", "{\"error\":\"Pin not found\"}");
}

// Ligar ou desligar apenas os 8 relés simultaneamente
void handleRelays() {
  if (!server.hasArg("state")) {
    server.send(400, "application/json", "{\"error\":\"Missing state\"}");
    return;
  }
  uint8_t val = server.arg("state").toInt() ? 1 : 0;

  for (size_t r = 0; r < 8; r++) {
    uint8_t relayPin = RELAY_PINS[r];
    for (size_t i = 0; i < PIN_COUNT; i++) {
      if (pins[i].pin == relayPin && pins[i].mode == PIN_MODE_OUTPUT) {
        pins[i].state = val;
        pins[i].pulseEnd = 0;
        digitalWrite(relayPin, val ? HIGH : LOW);
        break;
      }
    }
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"success\":true}");
}

// Ligar ou desligar todas as saídas configuradas
void handleAll() {
  if (!server.hasArg("state")) {
    server.send(400, "application/json", "{\"error\":\"Missing state\"}");
    return;
  }
  uint8_t val = server.arg("state").toInt() ? 1 : 0;

  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (pins[i].mode == PIN_MODE_OUTPUT) {
      pins[i].state = val;
      pins[i].pulseEnd = 0;
      digitalWrite(pins[i].pin, val ? HIGH : LOW);
    }
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"success\":true}");
}

void handleNotFound() {
  String uri = server.uri();
  if (uri.startsWith("/api/")) {
    server.send(404, "application/json", "{\"error\":\"Not found\"}");
    return;
  }
  handleRoot();
}
