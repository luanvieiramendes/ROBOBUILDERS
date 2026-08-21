#include "app_config.h"

AppConfig gConfig;
static Preferences prefs;

void loadConfig() {
  prefs.begin("painel", true);
  String s;
  s = prefs.getString("ssid", gConfig.wifi_ssid);
  s.toCharArray(gConfig.wifi_ssid, sizeof(gConfig.wifi_ssid));
  s = prefs.getString("pass", gConfig.wifi_pass);
  s.toCharArray(gConfig.wifi_pass, sizeof(gConfig.wifi_pass));

  s = prefs.getString("c1", gConfig.currency_1);
  s.toCharArray(gConfig.currency_1, sizeof(gConfig.currency_1));
  s = prefs.getString("c2", gConfig.currency_2);
  s.toCharArray(gConfig.currency_2, sizeof(gConfig.currency_2));
  s = prefs.getString("c3", gConfig.currency_3);
  s.toCharArray(gConfig.currency_3, sizeof(gConfig.currency_3));

  gConfig.curr1_enabled = prefs.getBool("c1en", gConfig.curr1_enabled);
  gConfig.curr2_enabled = prefs.getBool("c2en", gConfig.curr2_enabled);
  gConfig.curr3_enabled = prefs.getBool("c3en", gConfig.curr3_enabled);

  s = prefs.getString("city", gConfig.city);
  s.toCharArray(gConfig.city, sizeof(gConfig.city));

  gConfig.lat = prefs.getFloat("lat", gConfig.lat);
  gConfig.lon = prefs.getFloat("lon", gConfig.lon);
  gConfig.brightness = prefs.getUChar("bright", gConfig.brightness);
  gConfig.tz_offset = prefs.getChar("tz", gConfig.tz_offset);
  gConfig.dolar_interval = prefs.getUShort("dint", gConfig.dolar_interval);
  gConfig.weather_interval = prefs.getUShort("wint", gConfig.weather_interval);
  prefs.end();
  Serial.println("[Config] carregado");
}

void saveConfig() {
  prefs.begin("painel", false);
  prefs.putString("ssid", gConfig.wifi_ssid);
  prefs.putString("pass", gConfig.wifi_pass);
  prefs.putString("c1", gConfig.currency_1);
  prefs.putString("c2", gConfig.currency_2);
  prefs.putString("c3", gConfig.currency_3);
  prefs.putBool("c1en", gConfig.curr1_enabled);
  prefs.putBool("c2en", gConfig.curr2_enabled);
  prefs.putBool("c3en", gConfig.curr3_enabled);
  prefs.putString("city", gConfig.city);
  prefs.putFloat("lat", gConfig.lat);
  prefs.putFloat("lon", gConfig.lon);
  prefs.putUChar("bright", gConfig.brightness);
  prefs.putChar("tz", gConfig.tz_offset);
  prefs.putUShort("dint", gConfig.dolar_interval);
  prefs.putUShort("wint", gConfig.weather_interval);
  prefs.end();
  Serial.println("[Config] salvo");
}

void resetConfig() {
  prefs.begin("painel", false);
  prefs.clear();
  prefs.end();
}
