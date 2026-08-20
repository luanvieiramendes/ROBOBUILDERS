#include <Arduino.h>

#define LED_PIN 18
#define ON_MS 5000
#define OFF_MS 2000

unsigned long previousMillis = 0;
bool ledState = LOW;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long interval = ledState ? ON_MS : OFF_MS;

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
}