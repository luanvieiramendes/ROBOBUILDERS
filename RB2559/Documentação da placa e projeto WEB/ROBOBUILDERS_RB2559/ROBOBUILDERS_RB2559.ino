#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include "web_page.h"

// ==========================================
// CONFIGURAÇÃO DO PONTO DE ACESSO WI-FI
// ==========================================
const char* AP_SSID = "ROBOBUILDERS-RB2559";
const byte DNS_PORT = 53;

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

DNSServer dnsServer;
WebServer server(80);

// ==========================================
// MAPEAMENTO DAS GPIOS DA PLACA RB2559 (ESP-32 RELAY_X8)
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
  bool isRelay;
  uint8_t relayCh;
};

// Lista de pinos mapeados para a placa ESP-32 RELAY_X8
GPIOPin pins[] = {
  // --- 8 Relés de Potência Onboard (K1 a K8) ---
  { 32, PIN_MODE_OUTPUT, 0, 0, true, 1 }, // Relé 1 (K1)
  { 33, PIN_MODE_OUTPUT, 0, 0, true, 2 }, // Relé 2 (K2)
  { 25, PIN_MODE_OUTPUT, 0, 0, true, 3 }, // Relé 3 (K3)
  { 26, PIN_MODE_OUTPUT, 0, 0, true, 4 }, // Relé 4 (K4)
  { 27, PIN_MODE_OUTPUT, 0, 0, true, 5 }, // Relé 5 (K5)
  { 14, PIN_MODE_OUTPUT, 0, 0, true, 6 }, // Relé 6 (K6)
  { 12, PIN_MODE_OUTPUT, 0, 0, true, 7 }, // Relé 7 (K7)
  { 13, PIN_MODE_OUTPUT, 0, 0, true, 8 }, // Relé 8 (K8)

  // --- LED Onboard & Botão de Boot ---
  { 23, PIN_MODE_OUTPUT, 0, 0, false, 0 }, // LED Onboard (GPIO 23)
  { 0,  PIN_MODE_INPUT,  1, 0, false, 0 }, // Botão Boot / IO0 (GPIO 0)

  // --- GPIOs Livres para Teste e Expansão ---
  { 2,  PIN_MODE_OUTPUT, 0, 0, false, 0 },
  { 4,  PIN_MODE_OUTPUT, 0, 0, false, 0 },
  { 5,  PIN_MODE_OUTPUT, 0, 0, false, 0 },
  { 15, PIN_MODE_OUTPUT, 0, 0, false, 0 },
  { 16, PIN_MODE_OUTPUT, 0, 0, false, 0 },
  { 17, PIN_MODE_OUTPUT, 0, 0, false, 0 },
  { 18, PIN_MODE_OUTPUT, 0, 0, false, 0 },
  { 19, PIN_MODE_OUTPUT, 0, 0, false, 0 },
  { 21, PIN_MODE_OUTPUT, 0, 0, false, 0 },
  { 22, PIN_MODE_OUTPUT, 0, 0, false, 0 },

  // --- Pinos de Entrada Apenas (Input-Only) ---
  { 34, PIN_MODE_INPUT,  0, 0, false, 0 },
  { 35, PIN_MODE_INPUT,  0, 0, false, 0 },
  { 36, PIN_MODE_INPUT,  0, 0, false, 0 }, // VP
  { 39, PIN_MODE_INPUT,  0, 0, false, 0 }  // VN
};
const size_t PIN_COUNT = sizeof(pins) / sizeof(pins[0]);

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
void handleAll();
void handleRelay();
void handleRelayAll();
void handleNotFound();

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n==========================================");
  Serial.println("  ROBOBUILDERS RB2559 - TESTE DE PLACA    ");
  Serial.println("  Modelo: ESP-32 RELAY_X8 (8 CANAIS)      ");
  Serial.println("  LED Onboard: GPIO 23                    ");
  Serial.println("  Rele 1 (K1): GPIO 32  | Rele 5 (K5): GPIO 27");
  Serial.println("  Rele 2 (K2): GPIO 33  | Rele 6 (K6): GPIO 14");
  Serial.println("  Rele 3 (K3): GPIO 25  | Rele 7 (K7): GPIO 12");
  Serial.println("  Rele 4 (K4): GPIO 26  | Rele 8 (K8): GPIO 13");
  Serial.println("  Botao Boot:  GPIO 0 (IO0)              ");
  Serial.println("==========================================");

  // 1. Inicializar todas as GPIOs
  setupGPIOs();

  // 2. Configurar SoftAP Wi-Fi
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(AP_SSID, "");

  Serial.printf("[Wi-Fi] Rede Aberta Criada: %s\n", AP_SSID);
  Serial.printf("[Wi-Fi] IP do Servidor: %s\n", WiFi.softAPIP().toString().c_str());

  // 3. Iniciar Servidor DNS para Portal Captivo
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  // 4. Registrar Rotas da API REST & Páginas
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/toggle", HTTP_GET, handleToggle);
  server.on("/api/mode", HTTP_GET, handleMode);
  server.on("/api/pulse", HTTP_GET, handlePulse);
  server.on("/api/all", HTTP_GET, handleAll);
  server.on("/api/relay", HTTP_GET, handleRelay);
  server.on("/api/relay_all", HTTP_GET, handleRelayAll);

  // Redirecionamentos Captive Portal
  server.on("/generate_204", HTTP_GET, handleRoot);
  server.on("/gen_204", HTTP_GET, handleRoot);
  server.on("/hotspot-detect.html", HTTP_GET, handleRoot);
  server.on("/ncsi.txt", HTTP_GET, handleRoot);
  server.on("/connecttest.txt", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("[HTTP] Servidor Web Inicializado com Sucesso.");
}

// ==========================================
// LOOP PRINCIPAL
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
      if (p == 34 || p == 35 || p == 36 || p == 39) {
        pinMode(p, INPUT);
      } else {
        pinMode(p, INPUT_PULLUP);
      }
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
// HANDLERS HTTP REST
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
    if (pins[i].isRelay) {
      p["relay"] = pins[i].relayCh;
    }
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

  // Pinos 34, 35, 36 e 39 não podem ser output no ESP32
  if ((pinNum == 34 || pinNum == 35 || pinNum == 36 || pinNum == 39) && modeVal == 0) {
    server.send(400, "application/json", "{\"error\":\"Input-only pin cannot be set to output\"}");
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
        if (pinNum == 34 || pinNum == 35 || pinNum == 36 || pinNum == 39) {
          pinMode(pinNum, INPUT);
        } else {
          pinMode(pinNum, INPUT_PULLUP);
        }
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

void handleAll() {
  if (!server.hasArg("state")) {
    server.send(400, "application/json", "{\"error\":\"Missing state\"}");
    return;
  }
  uint8_t val = server.arg("state").toInt() ? 1 : 0;

  // Modifica apenas as GPIOs livres (não relés nem LED)
  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (!pins[i].isRelay && pins[i].pin != 23 && pins[i].mode == PIN_MODE_OUTPUT) {
      pins[i].state = val;
      pins[i].pulseEnd = 0;
      digitalWrite(pins[i].pin, val ? HIGH : LOW);
    }
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", "{\"success\":true}");
}

void handleRelay() {
  if (!server.hasArg("ch") || !server.hasArg("state")) {
    server.send(400, "application/json", "{\"error\":\"Missing ch or state\"}");
    return;
  }
  uint8_t ch = server.arg("ch").toInt();
  uint8_t val = server.arg("state").toInt() ? 1 : 0;

  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (pins[i].isRelay && pins[i].relayCh == ch) {
      pins[i].state = val;
      pins[i].pulseEnd = 0;
      digitalWrite(pins[i].pin, val ? HIGH : LOW);

      char res[64];
      snprintf(res, sizeof(res), "{\"relay\":%d,\"pin\":%d,\"val\":%d}", ch, pins[i].pin, val);
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "application/json", res);
      return;
    }
  }
  server.send(404, "application/json", "{\"error\":\"Relay channel not found\"}");
}

void handleRelayAll() {
  if (!server.hasArg("state")) {
    server.send(400, "application/json", "{\"error\":\"Missing state\"}");
    return;
  }
  uint8_t val = server.arg("state").toInt() ? 1 : 0;

  for (size_t i = 0; i < PIN_COUNT; i++) {
    if (pins[i].isRelay) {
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
