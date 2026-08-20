#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <ArduinoJson.h>
#include <lvgl.h>
#include "LGFX_ESP32_8048S070.h"

#define WIFI_SSID "ROBOBUILDERS"
#define WIFI_PASS "luan123*"

static LGFX tft;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[240 * 40];
static lv_color_t buf2[240 * 40];

static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *status_label;
static lv_obj_t *dolar_label;
static lv_obj_t *wifi_dot;
static lv_obj_t *weather_temp_label;
static lv_obj_t *weather_desc_label;
static lv_obj_t *weather_city_label;

static String dolarValue = "R$ --,--";
static String weatherTemp = "--";
static String weatherDesc = "----";
static String weatherCity = "Sao Paulo";

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((uint16_t *)&color_p->full, w * h);
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

void my_touch_read(lv_indev_drv_t *indev, lv_indev_data_t *data) {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

lv_obj_t *make_card(lv_obj_t *parent, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h, lv_color_t bg) {
  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_set_pos(card, x, y);
  lv_obj_set_size(card, w, h);
  lv_obj_set_style_bg_color(card, bg, 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(card, 24, 0);
  lv_obj_set_style_border_width(card, 0, 0);
  lv_obj_set_style_shadow_width(card, 30, 0);
  lv_obj_set_style_shadow_opa(card, 80, 0);
  lv_obj_set_style_shadow_color(card, lv_color_hex(0x000000), 0);
  lv_obj_set_style_pad_all(card, 20, 0);
  return card;
}

void make_accent_strip(lv_obj_t *parent, lv_coord_t h, lv_color_t color) {
  lv_obj_t *strip = lv_obj_create(parent);
  lv_obj_set_size(strip, 100, h);
  lv_obj_set_pos(strip, 0, 0);
  lv_obj_set_style_bg_color(strip, color, 0);
  lv_obj_set_style_radius(strip, 0, 0);
  lv_obj_set_style_border_width(strip, 0, 0);
}

void create_ui() {
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x070A12), 0);
  lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0x0E1420), 0);
  lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, 0);
  lv_obj_set_style_bg_main_stop(scr, 0, 0);
  lv_obj_set_style_bg_grad_stop(scr, 255, 0);

  lv_obj_t *header = lv_obj_create(scr);
  lv_obj_set_size(header, 800, 84);
  lv_obj_set_pos(header, 0, 0);
  lv_obj_set_style_bg_color(header, lv_color_hex(0x06080D), 0);
  lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(header, 0, 0);
  lv_obj_set_style_border_width(header, 0, 0);
  lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
  lv_obj_set_style_border_width(header, 1, 0);
  lv_obj_set_style_border_color(header, lv_color_hex(0x1E2A3A), 0);
  lv_obj_set_style_pad_all(header, 0, 0);

  lv_obj_t *title = lv_label_create(header);
  lv_label_set_text(title, "PAINEL FINANCEIRO");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xF8FAFC), 0);
  lv_obj_set_style_text_letter_space(title, 2, 0);
  lv_obj_align(title, LV_ALIGN_LEFT_MID, 32, 0);

  lv_obj_t *title_bar = lv_obj_create(header);
  lv_obj_set_size(title_bar, 6, 44);
  lv_obj_set_pos(title_bar, 14, 20);
  lv_obj_set_style_bg_color(title_bar, lv_color_hex(0x22D3EE), 0);
  lv_obj_set_style_radius(title_bar, 3, 0);
  lv_obj_set_style_border_width(title_bar, 0, 0);

  lv_obj_t *title_sub = lv_label_create(header);
  lv_label_set_text(title_sub, "Horario e cambio em tempo real");
  lv_obj_set_style_text_font(title_sub, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(title_sub, lv_color_hex(0x7A8699), 0);
  lv_obj_align(title_sub, LV_ALIGN_LEFT_MID, 32, 30);

  wifi_dot = lv_obj_create(header);
  lv_obj_set_size(wifi_dot, 12, 12);
  lv_obj_set_style_bg_color(wifi_dot, lv_color_hex(0xFF5252), 0);
  lv_obj_set_style_radius(wifi_dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_border_width(wifi_dot, 0, 0);
  lv_obj_set_style_shadow_width(wifi_dot, 10, 0);
  lv_obj_set_style_shadow_color(wifi_dot, lv_color_hex(0xFF5252), 0);
  lv_obj_align(wifi_dot, LV_ALIGN_RIGHT_MID, -140, 0);

  status_label = lv_label_create(header);
  lv_label_set_text(status_label, "Conectando...");
  lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(status_label, lv_color_hex(0x7A8699), 0);
  lv_obj_align(status_label, LV_ALIGN_RIGHT_MID, -32, 0);

  lv_obj_t *time_card = make_card(scr, 28, 108, 360, 300, lv_color_hex(0x0F1622));
  lv_obj_set_style_border_color(time_card, lv_color_hex(0x1E2A3A), 0);
  lv_obj_set_style_border_width(time_card, 1, 0);
  make_accent_strip(time_card, 4, lv_color_hex(0x22D3EE));

  lv_obj_t *time_caption = lv_label_create(time_card);
  lv_label_set_text(time_caption, "HORARIO LOCAL");
  lv_obj_set_style_text_font(time_caption, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(time_caption, lv_color_hex(0x22D3EE), 0);
  lv_obj_set_style_text_letter_space(time_caption, 3, 0);
  lv_obj_align(time_caption, LV_ALIGN_TOP_MID, 0, 18);

  time_label = lv_label_create(time_card);
  lv_label_set_text(time_label, "--:--:--");
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(time_label, lv_color_hex(0xF8FAFC), 0);
  lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 10);

  date_label = lv_label_create(time_card);
  lv_label_set_text(date_label, "");
  lv_obj_set_style_text_font(date_label, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(date_label, lv_color_hex(0x7A8699), 0);
  lv_obj_align(date_label, LV_ALIGN_BOTTOM_MID, 0, -20);

  lv_obj_t *weather_card = make_card(scr, 410, 108, 170, 300, lv_color_hex(0x0F1622));
  lv_obj_set_style_border_color(weather_card, lv_color_hex(0x1E2A3A), 0);
  lv_obj_set_style_border_width(weather_card, 1, 0);
  make_accent_strip(weather_card, 4, lv_color_hex(0xFFB300));

  lv_obj_t *weather_caption = lv_label_create(weather_card);
  lv_label_set_text(weather_caption, "CLIMA");
  lv_obj_set_style_text_font(weather_caption, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(weather_caption, lv_color_hex(0xFFB300), 0);
  lv_obj_set_style_text_letter_space(weather_caption, 3, 0);
  lv_obj_align(weather_caption, LV_ALIGN_TOP_MID, 0, 18);

  weather_city_label = lv_label_create(weather_card);
  lv_label_set_text(weather_city_label, weatherCity.c_str());
  lv_obj_set_style_text_font(weather_city_label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(weather_city_label, lv_color_hex(0x7A8699), 0);
  lv_obj_align(weather_city_label, LV_ALIGN_TOP_MID, 0, 52);

  weather_temp_label = lv_label_create(weather_card);
  lv_label_set_text(weather_temp_label, "--\xC2\xB0");
  lv_obj_set_style_text_font(weather_temp_label, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(weather_temp_label, lv_color_hex(0xFFB300), 0);
  lv_obj_align(weather_temp_label, LV_ALIGN_CENTER, 0, 10);

  weather_desc_label = lv_label_create(weather_card);
  lv_label_set_text(weather_desc_label, "----");
  lv_obj_set_style_text_font(weather_desc_label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(weather_desc_label, lv_color_hex(0x7A8699), 0);
  lv_obj_align(weather_desc_label, LV_ALIGN_BOTTOM_MID, 0, -20);

  lv_obj_t *dolar_card = make_card(scr, 602, 108, 170, 300, lv_color_hex(0x0F1622));
  lv_obj_set_style_border_color(dolar_card, lv_color_hex(0x1E2A3A), 0);
  lv_obj_set_style_border_width(dolar_card, 1, 0);
  make_accent_strip(dolar_card, 4, lv_color_hex(0x00E676));

  lv_obj_t *dolar_caption = lv_label_create(dolar_card);
  lv_label_set_text(dolar_caption, "CAMBIO");
  lv_obj_set_style_text_font(dolar_caption, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(dolar_caption, lv_color_hex(0x00E676), 0);
  lv_obj_set_style_text_letter_space(dolar_caption, 3, 0);
  lv_obj_align(dolar_caption, LV_ALIGN_TOP_MID, 0, 18);

  lv_obj_t *dolar_pair = lv_label_create(dolar_card);
  lv_label_set_text(dolar_pair, "USD / BRL");
  lv_obj_set_style_text_font(dolar_pair, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(dolar_pair, lv_color_hex(0x5A6A80), 0);
  lv_obj_align(dolar_pair, LV_ALIGN_TOP_MID, 0, 52);

  dolar_label = lv_label_create(dolar_card);
  lv_label_set_text(dolar_label, dolarValue.c_str());
  lv_obj_set_style_text_font(dolar_label, &lv_font_montserrat_32, 0);
  lv_obj_set_style_text_color(dolar_label, lv_color_hex(0x00E676), 0);
  lv_obj_align(dolar_label, LV_ALIGN_CENTER, 0, 30);

  lv_obj_t *footer = lv_label_create(scr);
  lv_label_set_text(footer, "Atualizado a cada 60s  |  Hora sincronizada via NTP");
  lv_obj_set_style_text_font(footer, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(footer, lv_color_hex(0x5A6A80), 0);
  lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void update_clock(lv_timer_t *timer) {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    static const char *weekdays[] = {"Domingo", "Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado"};
    static const char *months[] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"};
    char dateStr[64];
    snprintf(dateStr, sizeof(dateStr), "%s, %02d de %s de %d",
             weekdays[timeinfo.tm_wday], timeinfo.tm_mday, months[timeinfo.tm_mon], timeinfo.tm_year + 1900);

    lv_label_set_text(time_label, timeStr);
    lv_label_set_text(date_label, dateStr);
  }
}

void update_dolar(lv_timer_t *timer) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.setTimeout(10000);
  http.begin("https://economia.awesomeapi.com.br/json/last/USD-BRL");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (!err) {
      const char *bid = doc["USDBRL"]["bid"];
      dolarValue = String("R$ ") + bid;
      lv_label_set_text(dolar_label, dolarValue.c_str());
      Serial.println("Dolar: " + dolarValue);
    } else {
      Serial.println("Erro ao parsear JSON");
    }
  } else {
    Serial.printf("Erro HTTP: %d\n", httpCode);
  }
  http.end();
}

void update_weather(lv_timer_t *timer) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.setTimeout(10000);
  http.begin("https://api.open-meteo.com/v1/forecast?latitude=-23.5505&longitude=-46.6333&current_weather=true");
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (!err) {
      float temp = doc["current_weather"]["temperature"].as<float>();
      int wcode = doc["current_weather"]["weathercode"].as<int>();

      static const char *descs[] = {"Sol", "Predom. sol", "Parcial. nuvens", "Nublado", "Nevoeiro", "Chuvisco", "Chuva", "Neve", "Trovoadas"};
      const char *desc = "N/A";
      if (wcode == 0) desc = descs[0];
      else if (wcode == 1 || wcode == 2) desc = descs[1];
      else if (wcode == 3) desc = descs[2];
      else if (wcode == 45 || wcode == 48) desc = descs[3];
      else if (wcode >= 51 && wcode <= 57) desc = descs[4];
      else if (wcode >= 61 && wcode <= 67) desc = descs[5];
      else if (wcode >= 71 && wcode <= 77) desc = descs[6];
      else if (wcode >= 95) desc = descs[7];

      char buf[16];
      snprintf(buf, sizeof(buf), "%.0f\xC2\xB0", temp);
      weatherTemp = buf;
      weatherDesc = desc;

      lv_label_set_text(weather_temp_label, weatherTemp.c_str());
      lv_label_set_text(weather_desc_label, weatherDesc.c_str());
      Serial.println("Clima: " + weatherTemp + " " + weatherDesc);
    } else {
      Serial.println("Erro ao parsear clima JSON");
    }
  } else {
    Serial.printf("Erro HTTP clima: %d\n", httpCode);
  }
  http.end();
}

void setup() {
  Serial.begin(115200);

  lv_init();
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, 240 * 40);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 800;
  disp_drv.ver_res = 480;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touch_read;
  lv_indev_drv_register(&indev_drv);

  create_ui();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    lv_timer_handler();
    delay(10);
  }

if (WiFi.status() == WL_CONNECTED) {
    lv_obj_set_style_bg_color(wifi_dot, lv_color_hex(0x00E676), 0);
    lv_obj_set_style_shadow_color(wifi_dot, lv_color_hex(0x00E676), 0);
    char buf[64];
    snprintf(buf, sizeof(buf), "IP: %s", WiFi.localIP().toString().c_str());
    lv_label_set_text(status_label, buf);
    Serial.println("WiFi conectado!");
  } else {
    lv_obj_set_style_bg_color(wifi_dot, lv_color_hex(0xFF5252), 0);
    lv_obj_set_style_shadow_color(wifi_dot, lv_color_hex(0xFF5252), 0);
    lv_label_set_text(status_label, "Falha no WiFi");
    Serial.println("Falha no WiFi");
  }

  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    lv_timer_handler();
    delay(500);
  }
  Serial.println("Tempo sincronizado!");

  lv_timer_create(update_clock, 1000, NULL);
  lv_timer_create(update_dolar, 60000, NULL);
  lv_timer_create(update_weather, 600000, NULL);
  update_dolar(NULL);
  update_weather(NULL);
}

void loop() {
  lv_tick_inc(5);
  lv_timer_handler();
  delay(5);
}