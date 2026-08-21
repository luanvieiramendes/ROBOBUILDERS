#include "ota_updater.h"
#include "version.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ArduinoJson.h>

OtaInfo gOta;

static String normalizeVersion(String v){
  v.replace("v",""); v.replace("V",""); v.trim();
  return v;
}
static int versionToCode(String v){
  // "1.2.3" -> 123
  v = normalizeVersion(v);
  int maj=0, min=0, pat=0;
  sscanf(v.c_str(), "%d.%d.%d", &maj, &min, &pat);
  return maj*100 + min*10 + pat; // suporta até 9.9.9, bom para 1.1.0
}

String otaGetCurrentVersion(){ return String(FIRMWARE_VERSION); }

void otaInit(){
  gOta.current = String(FIRMWARE_VERSION);
  gOta.state = OTA_IDLE;
  Serial.printf("[OTA] versao atual %s (%d)\n", FIRMWARE_VERSION, FIRMWARE_VERSION_CODE);
}

bool otaCheck(bool showLog){
  if(WiFi.status()!=WL_CONNECTED){
    gOta.error = "WiFi desconectado";
    gOta.state = OTA_FAILED;
    return false;
  }
  gOta.state = OTA_CHECKING;
  gOta.error = "";
  if(showLog) Serial.println("[OTA] verificando release mais recente...");

  HTTPClient http;
  http.setTimeout(10000);
  http.addHeader("User-Agent", "ESP32-OTA");
  http.addHeader("Accept", "application/vnd.github+json");
  // GitHub API exige User-Agent
  http.begin(GITHUB_API_LATEST);
  int code = http.GET();
  if(code != 200){
    gOta.error = "HTTP " + String(code);
    gOta.state = OTA_FAILED;
    Serial.printf("[OTA] falha HTTP %d\n", code);
    http.end();
    return false;
  }
  String payload = http.getString();
  http.end();
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload);
  if(err){
    gOta.error = "JSON erro";
    gOta.state = OTA_FAILED;
    return false;
  }
  String tag = doc["tag_name"] | "";
  if(tag.length()==0){
    gOta.error = "sem tag_name";
    gOta.state = OTA_FAILED;
    return false;
  }
  gOta.latest = normalizeVersion(tag);
  // pega asset .bin (primeiro que termina com .bin)
  String dl = "";
  JsonArray assets = doc["assets"];
  for(JsonObject a : assets){
    String name = a["name"] | "";
    if(name.endsWith(".bin")){
      dl = a["browser_download_url"] | "";
      break;
    }
  }
  // fallback: usa url do zip? tenta pegar mesmo se não for bin
  if(dl.length()==0 && assets.size()>0){
    dl = assets[0]["browser_download_url"] | "";
  }
  gOta.downloadUrl = dl;
  int latestCode = versionToCode(tag);
  int curCode = FIRMWARE_VERSION_CODE;
  Serial.printf("[OTA] atual=%s (%d) latest=%s (%d) url=%s\n", gOta.current.c_str(), curCode, gOta.latest.c_str(), latestCode, dl.c_str());
  if(latestCode > curCode && dl.length()>0){
    gOta.state = OTA_NO_UPDATE; // na verdade tem update disponível
    // marca como pronto para atualizar
    gOta.error = "Atualizacao disponivel: " + tag;
    if(showLog) Serial.println("[OTA] " + gOta.error);
    return true; // há atualização
  } else {
    gOta.state = OTA_NO_UPDATE;
    gOta.error = "Ja esta na ultima versao";
    if(showLog) Serial.println("[OTA] ja atualizado");
    return false;
  }
}

bool otaUpdate(){
  if(gOta.downloadUrl.length()==0){
    gOta.error = "sem URL";
    gOta.state = OTA_FAILED;
    return false;
  }
  if(WiFi.status()!=WL_CONNECTED){
    gOta.error = "WiFi desconectado";
    gOta.state = OTA_FAILED;
    return false;
  }
  gOta.state = OTA_UPDATING;
  gOta.progress = 0;
  Serial.println("[OTA] iniciando download " + gOta.downloadUrl);
  HTTPClient http;
  http.setTimeout(20000);
  http.addHeader("User-Agent", "ESP32-OTA");
  http.begin(gOta.downloadUrl);
  // GitHub redireciona, HTTPClient segue por padrão?
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  int code = http.GET();
  if(code != 200){
    gOta.error = "download HTTP " + String(code);
    gOta.state = OTA_FAILED;
    Serial.printf("[OTA] download falha %d\n", code);
    http.end();
    return false;
  }
  int len = http.getSize();
  if(len <= 0){
    // tenta com header Content-Length
    len = http.getSize();
  }
  Serial.printf("[OTA] tamanho %d\n", len);
  if(!Update.begin(len > 0 ? len : UPDATE_SIZE_UNKNOWN)){
    gOta.error = Update.errorString();
    gOta.state = OTA_FAILED;
    http.end();
    return false;
  }
  WiFiClient *stream = http.getStreamPtr();
  uint8_t buff[4096];
  int written = 0;
  unsigned long last = millis();
  while(http.connected() && (len > 0 || len == -1)){
    size_t size = stream->available();
    if(size){
      int c = stream->readBytes(buff, (size > sizeof(buff)) ? sizeof(buff) : size);
      if(Update.write(buff, c) != c){
        gOta.error = Update.errorString();
        gOta.state = OTA_FAILED;
        Update.abort();
        http.end();
        return false;
      }
      written += c;
      if(len > 0){
        gOta.progress = (written * 100) / len;
      }
      if(millis() - last > 500){
        Serial.printf("[OTA] %d/%d (%d%%)\n", written, len, gOta.progress);
        last = millis();
      }
      if(len > 0 && written >= len) break;
    }
    delay(1);
  }
  http.end();
  if(Update.end(true)){
    gOta.state = OTA_SUCCESS;
    gOta.progress = 100;
    Serial.println("[OTA] sucesso, reiniciando...");
    delay(500);
    ESP.restart();
    return true;
  } else {
    gOta.error = Update.errorString();
    gOta.state = OTA_FAILED;
    Serial.printf("[OTA] erro final %s\n", gOta.error.c_str());
    return false;
  }
}

void otaLoop(){}
