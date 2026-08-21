#pragma once
#include <Arduino.h>

enum OtaState { OTA_IDLE, OTA_CHECKING, OTA_NO_UPDATE, OTA_UPDATING, OTA_SUCCESS, OTA_FAILED };
struct OtaInfo {
  OtaState state = OTA_IDLE;
  String current = "";
  String latest = "";
  String downloadUrl = "";
  String error = "";
  int progress = 0;
};

extern OtaInfo gOta;

void otaInit();
bool otaCheck(bool showLog = true); // verifica se há release mais novo
bool otaUpdate(); // baixa e grava (chamado após check) - roda em task background
void otaRequestUpdate(); // agenda o update em background (nao bloqueia o webserver)
String otaGetCurrentVersion();
void otaLoop(); // para uso futuro com timer
