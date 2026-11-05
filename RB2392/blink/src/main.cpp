#include <Arduino.h>

// Pinos configuraveis pelo platformio.ini (build_flags)
#ifndef LED_PIN
#define LED_PIN 15       // LED simples onboard
#endif

#ifndef RGB_LED_PIN
#define RGB_LED_PIN 8    // LED RGB WS2812 onboard
#endif

static const uint32_t BLINK_INTERVAL_MS = 500;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  rgbLedWrite(RGB_LED_PIN, 0, 0, 0);

  Serial.println();
  Serial.println("=== Blink ESP32-C6 Mini ===");
  Serial.printf("LED simples : GPIO%d\n", LED_PIN);
  Serial.printf("LED RGB     : GPIO%d\n", RGB_LED_PIN);
}

void loop() {
  // Liga o LED simples e o RGB (verde)
  digitalWrite(LED_PIN, HIGH);
  rgbLedWrite(RGB_LED_PIN, 0, 48, 0);
  Serial.println("LED ON");
  delay(BLINK_INTERVAL_MS);

  // Desliga os dois
  digitalWrite(LED_PIN, LOW);
  rgbLedWrite(RGB_LED_PIN, 0, 0, 0);
  Serial.println("LED OFF");
  delay(BLINK_INTERVAL_MS);
}
