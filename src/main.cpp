#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "LGFX_ESP32_8048S070.h"

#define WIFI_SSID "ROBOBUILDERS"
#define WIFI_PASS "luan123*"

static LGFX tft;

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  tft.setFont(&fonts::Font2);
  tft.setTextSize(2);
  tft.drawString("Conectando WiFi...", 400, 220);
  Serial.println("Conectando WiFi...");

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado! IP: " + WiFi.localIP().toString());
    tft.drawString("Conectado! IP: " + WiFi.localIP().toString(), 400, 220);
  } else {
    Serial.println("\nFalha ao conectar WiFi");
    tft.drawString("Falha ao conectar WiFi", 400, 220);
  }

  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Aguardando NTP...");
    delay(1000);
  }
  Serial.println("Tempo sincronizado!");
}

void loop() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    char dateStr[32];
    static const char* weekdays[] = {"DOM", "SEG", "TER", "QUA", "QUI", "SEX", "SAB"};
    static const char* months[] = {"JAN", "FEV", "MAR", "ABR", "MAI", "JUN", "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"};
    snprintf(dateStr, sizeof(dateStr), "%s, %02d %s %d",
             weekdays[timeinfo.tm_wday], timeinfo.tm_mday, months[timeinfo.tm_mon], timeinfo.tm_year + 1900);

    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(middle_center);

    tft.setFont(&fonts::Font7);
    tft.setTextSize(1);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString(timeStr, 400, 200);

    tft.setFont(&fonts::Font2);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(dateStr, 400, 320);

    tft.setTextDatum(top_left);
  }
  delay(1000);
}