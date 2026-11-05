/*
 * Blink — ESP32-C6 Mini / ESP32-C6 SuperMini
 * ------------------------------------------------------------
 * Versão para Arduino IDE.
 *
 * Placa: "ESP32C6 Dev Module" (ou "ESP32-C6 DevKitM-1")
 * Core ESP32: 3.x ou superior (necessário para rgbLedWrite)
 *
 * LEDs onboard:
 *   GPIO15 -> LED simples (status)
 *   GPIO8  -> LED RGB WS2812
 *
 * Se a sua placa usar outros pinos, altere as constantes abaixo.
 */

#include <Arduino.h>

#define LED_PIN 15       // LED simples onboard
#define RGB_LED_PIN 8    // LED RGB WS2812 onboard

#define BLINK_INTERVAL_MS 500

// rgbLedWrite() existe a partir do core ESP32 3.0.0
static void setRgb(uint8_t r, uint8_t g, uint8_t b) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
  rgbLedWrite(RGB_LED_PIN, r, g, b);
#else
  (void)r;
  (void)g;
  (void)b;
#endif
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  setRgb(0, 0, 0);

  Serial.println();
  Serial.println("=== Blink ESP32-C6 Mini (Arduino IDE) ===");
  Serial.printf("LED simples : GPIO%d\n", LED_PIN);
  Serial.printf("LED RGB     : GPIO%d\n", RGB_LED_PIN);
}

void loop() {
  // Liga o LED simples e o RGB (verde)
  digitalWrite(LED_PIN, HIGH);
  setRgb(0, 48, 0);
  Serial.println("LED ON");
  delay(BLINK_INTERVAL_MS);

  // Desliga os dois
  digitalWrite(LED_PIN, LOW);
  setRgb(0, 0, 0);
  Serial.println("LED OFF");
  delay(BLINK_INTERVAL_MS);
}
