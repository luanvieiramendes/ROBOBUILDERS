#pragma once
#include <Arduino.h>
#include <Preferences.h>

struct AppConfig {
  char wifi_ssid[64] = "ROBOBUILDERS";
  char wifi_pass[64] = "luan123*";
  // Moedas: awesomeapi pairs. Até 3 simultâneas no painel
  char currency_1[16] = "USD-BRL";
  char currency_2[16] = "EUR-BRL";
  char currency_3[16] = "BTC-BRL";
  bool curr1_enabled = true;
  bool curr2_enabled = true;
  bool curr3_enabled = true;
  char city[64] = "Sao Paulo";
  float lat = -23.5505f;
  float lon = -46.6333f;
  uint8_t brightness = 180;
  int8_t tz_offset = -3; // -3 Brasil
  uint16_t dolar_interval = 60;   // segundos
  uint16_t weather_interval = 600; // segundos
};

extern AppConfig gConfig;

void loadConfig();
void saveConfig();
void resetConfig();
