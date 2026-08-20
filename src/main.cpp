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

static String dolarValue = "R$ --,--";

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

void create_ui() {
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x1E1E2E), 0);

  time_label = lv_label_create(lv_scr_act());
  lv_label_set_text(time_label, "--:--:--");
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(time_label, lv_color_hex(0x89B4FA), 0);
  lv_obj_center(time_label);

  date_label = lv_label_create(lv_scr_act());
  lv_label_set_text(date_label, "");
  lv_obj_set_style_text_font(date_label, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(date_label, lv_color_hex(0xA6ADC8), 0);
  lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 70);

  status_label = lv_label_create(lv_scr_act());
  lv_label_set_text(status_label, "");
  lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(status_label, lv_color_hex(0x6C7086), 0);
  lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -20);

  lv_obj_t *dolar_title = lv_label_create(lv_scr_act());
  lv_label_set_text(dolar_title, "DOLAR");
  lv_obj_set_style_text_font(dolar_title, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(dolar_title, lv_color_hex(0x6C7086), 0);
  lv_obj_align(dolar_title, LV_ALIGN_CENTER, 0, -60);

  dolar_label = lv_label_create(lv_scr_act());
  lv_label_set_text(dolar_label, dolarValue.c_str());
  lv_obj_set_style_text_font(dolar_label, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(dolar_label, lv_color_hex(0xA6E3A1), 0);
  lv_obj_align(dolar_label, LV_ALIGN_CENTER, 0, -10);
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
  lv_label_set_text(status_label, "Conectando WiFi...");

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    lv_timer_handler();
    delay(10);
  }

  if (WiFi.status() == WL_CONNECTED) {
    char buf[64];
    snprintf(buf, sizeof(buf), "WiFi: %s", WiFi.localIP().toString().c_str());
    lv_label_set_text(status_label, buf);
    Serial.println("WiFi conectado!");
  } else {
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
  update_dolar(NULL);
}

void loop() {
  lv_tick_inc(5);
  lv_timer_handler();
  delay(5);
}