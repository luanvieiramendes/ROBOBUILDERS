# Painel Financeiro — ESP32-8048S070 (7" 800×480) + LVGL + WebServer + OTA

Painel em tempo real para **ESP32-S3 8048S070 7" RGB 800×480** com LovyanGFX + LVGL 8.3, WebServer de configuração, WiFiManager com scan, clima e câmbio ao vivo, e **OTA automático via GitHub Releases + GitHub Actions** (auto-update em task dedicada, com fallback sem API e tolerante a rate-limit). Projetado para ser **reutilizável em outros projetos**: copie `src/app_config.*`, `src/LGFX_ESP32_8048S070.h`, `src/web_server.*`, `src/ota_updater.*` e `src/version.h`.

![ESP32-8048S070](https://img.shields.io/badge/board-ESP32--8048S070-yellow?style=flat)
![LVGL](https://img.shields.io/badge/LVGL-8.3-blue)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Arduino-orange)
![OTA](https://img.shields.io/badge/OTA-GitHub%20Releases-green)

---

## Índice
- [Features](#features)
- [Hardware](#hardware)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Começando](#começando)
- [WiFi Manager](#wifimanager-implementado)
- [WebServer](#webserver)
- [Configurações do Display](#configurações-do-display)
- [OTA Update com GitHub Actions](#ota-update-com-github-actions)
- [Troubleshooting OTA](#troubleshooting-ota)
- [API](#api)
- [Reutilizando em Outros Projetos](#reutilizando-em-outros-projetos)
- [Troubleshooting Display](#troubleshooting-display)
- [Roadmap](#roadmap)

---

## Features

| Módulo | Detalhe |
|---|---|
| **Display** | 7" 800×480 RGB LovyanGFX, timings validados `8/2/43 + 8/2/12 @12MHz`, `pushImage` + LVGL `16-bit`, brilho `PWM 44100Hz`, buffer `800×32` single IRAM |
| **UI** | LVGL cards proporcionais — `≤3 moedas` linha única, `>3` grid `3×2` na metade direita, `Horário` + `Clima` sempre visíveis, tema claro/escuro |
| **WiFiManager** | Scan `WiFi.scanNetworks()`, lista com RSSI/segurança, clique preenche SSID, senha só, fallback `ROBOBUILDERS` + `AP Painel-Config 192.168.4.1` |
| **WebServer** | SPA em `PROGMEM` com tabs `Dashboard/Moedas/Clima/Sistema`, `IBGE 5570 municípios`, `AwesomeAPI` + `Open-Meteo` com preview ao vivo, `/api/*` |
| **Clima** | `Open-Meteo` por `lat/lon`, busca `IBGE estados→municípios` + 27 capitais 1-clique + geocoding, preview `°C` antes de salvar |
| **Câmbio** | Até 6 pares `AwesomeAPI` `USD-BRL,EUR-BRL,BTC-BRL...` request único `json/last/`, BTC cabe com fonte adaptativa (`R$ 401k`), metade da tela `3×2` |
| **OTA** | Check `api.github.com/repos/<user>/<repo>/releases/latest` com fallback por redirect (sem rate-limit), compara `FIRMWARE_VERSION_CODE`, download `browser_download_url` `.bin` via `Update` em **task FreeRTOS dedicada (32KB)**, auto-update ~30s + manual no WebServer |
| **GitHub Action** | `pio run` → `firmware.bin` → `firmware-v*.*.*.bin` + `firmware-latest.bin` → `action-gh-release` em tags `v*.*.*` |

---

## Hardware

- **Board:** `ESP32-8048S070 / ESP32-S3 800*480` (Sunton) — `QIO OPI PSRAM 8MB`, `Flash 8MB QIO`
- **Driver:** `ST7262` RGB 16-bit (5R/6G/5B), `GT911` touch `I2C 19/20`
- **Alimentação:** **5V 2A** via USB-C; 3.3V fraco deixa tela lavada (ver Troubleshooting)
- **Pinos RGB** (`src/LGFX_ESP32_8048S070.h:33-76`):

```
B0-B4: 15,7,6,5,4
G0-G5: 9,46,3,8,16,1
R0-R4: 14,21,47,48,45
DE/VS/HS/PCLK: 41/40/39/42
BL: 2 (PWM 44100Hz ch7)
TOUCH SDA/SCL/RST: 19/20/38 @400kHz 0x14
```

Esquemático e demo original em `7.0inch_ESP32-8048S070/1-Demo/Demo_Arduino/3_3-4_TFT-LVGL-Widgets/`.

---

## Estrutura do Projeto

```
Projeto - Painel Financeiro com ESP32-S3 e Atualização OTA/
├── platformio.ini
├── src/
│   ├── main.cpp                 # setup/loop, LVGL, WiFi, NTP, timers, gNeedsRebuild
│   ├── LGFX_ESP32_8048S070.h    # LovyanGFX Bus_RGB + Panel_RGB + Light_PWM + GT911
│   ├── lv_conf.h                # LVGL 8.3 config (COLOR_DEPTH 16, fonts 12/14/16/20/24/32/48)
│   ├── app_config.h/.cpp        # Preferences NVS: moedas(6), cidade/lat/lon, brilho, fuso, intervalos, display_light
│   ├── web_server.h/.cpp        # WebServer :80, SPA PROGMEM, /api/*, WiFi scan, mirror
│   ├── ota_updater.h/.cpp       # GitHub Releases check + Update (task 32KB, auto)
│   └── version.h                # FIRMWARE_VERSION "2.0.7" / CODE 207
├── .github/workflows/build-release.yml
```

---

## Começando

### 1. Requisitos
- VS Code + PlatformIO
- `esp32` platform `6.13.0` (`espressif32`), `LovyanGFX@1.1.15`, `lvgl@8.3.11`, `ArduinoJson@7`

### 2. Clonar e buildar

```bash
git clone https://github.com/luanvieiramendes/ROBOBUILDERS.git
cd "RB1212/Projeto - Painel Financeiro com ESP32-S3 e Atualização OTA"
pio run -e esp32-8048s070 --target upload --upload-port COM5
pio device monitor --port COM5 --baud 115200
```

### 3. Primeiro boot
- Log: `[Config] carregado` → `[WiFi] Conectando em 'ROBOBUILDERS' ...` → `WiFi conectado! IP: 192.168.1.57`
- Se falhar: `Falha WiFi - iniciando AP "Painel-Config" 12345678` → conecte e acesse `http://192.168.4.1/`
- Tela mostra `PAINEL FINANCEIRO`, `10:15:57`, `24° São Paulo`, `R$ 5,18` e footer `v2.0.7`

---

## WiFiManager Implementado

> **Não usa `WiFiManager` externo.** Implementação leve em `src/main.cpp:544` + `src/web_server.cpp:384` + `src/app_config.*` — sem portal captivo, só `Preferences` + `WebServer`.

### Fluxo

```
loadConfig() (NVS "painel")
  → WiFi.begin(gConfig.ssid, pass) 15s
  → fallback ROBOBUILERS 15s se SSID salvo falhar
  → se ainda falhar: WiFi.softAP("Painel-Config","12345678") + webServerInit()
  → sempre webServerInit() (STA ou AP)
```

### Scan (sem digitar SSID)

- **Endpoint** `GET /api/scan` (`src/web_server.cpp:384`) → `WiFi.scanNetworks()` → `[{ssid,rssi,encryption,channel}]`
- **Web** `Sistema → WIFI → 🔍 Buscar redes próximas` → `scanWifi()` fetch lista → `wifi-item` `🔒/🔓` + `▂▄▆█` → `selectWifi(ssid)` preenche `#ssid` + foca senha → `Salvar e Conectar` `POST /api/config {ssid,pass}` → `WiFi.begin()` sem reiniciar.

### Persistência

| Chave NVS | Campo | Padrão |
|---|---|---|
| `ssid/pass` | `wifi_ssid/pass` | `ROBOBUILDERS/luan123*` |
| `c1..c6 / c1en..c6en` | `currency_1..6` | `USD-BRL/EUR-BRL/BTC-BRL...` |
| `city/lat/lon` | clima | `Sao Paulo -23.5505/-46.6333` |
| `bright/tz/dint/wint` | sistema | `180/-3/60/600` |
| `dlight` | `display_light` | `false` (noturno) |

Trocar WiFi tenta reconectar 20s; se falhar mantém AP. Veja `Serial` `11:17:53 [WiFi] Scan redes proximas:`.

### Reutilizar

Copie `app_config.h/.cpp` e no `setup()`:

```cpp
loadConfig();
WiFi.begin(gConfig.wifi_ssid, gConfig.wifi_pass);
webServerInit(); // registra /api/scan
```

---

## WebServer

SPA single-file em `PROGMEM` (`src/web_server.cpp:16` `R"rawliteral(...)"`), sem `LittleFS`. Tabs: `Dashboard | Moedas (6) | Clima | Sistema/Rede`. Tema claro/escuro via `data-theme` + `localStorage`.

### Topo (sempre visível)

- `PAINEL FINANCEIRO` + `🌙/☀️ Escuro/Claro` `toggleTheme()` → `POST /api/config {dlight}` → ESP `gConfig.display_light` + `gNeedsRebuild` → `create_ui()` recria com cores claras + site `data-theme="light"`
- `👁️ Tela` → `toggleMirror()` mostra/esconde `#globalMirror`
- `IP: 192.168.1.57` `Conectado`

### Espelhamento (idêntico proporcional)

`#globalMirror` (`src/web_server.cpp:24-70`) — `800×480` `scale(0.55)` `border:2px #d4a017` recolhido por padrão (`display:none`) para não ocupar espaço. Réplica fiel com `updateMirror(j)` a cada `loadData()` (2s) — `time/date/city/weather/câmbio` com mesma paleta `colCard/colBorder` e layout `≤3` linha única, `>3` `3×2` metade direita. Sem fundo preto extra.

### Moedas (6, metade da tela)

- Até **6 moedas** `src/app_config.h:9` em `3×2` na metade direita (`src/main.cpp:83`). `≤3` → linha única proporcional (`timeW 3×, clima 2×, moeda 2×`). `>3` → `time 340` esquerda + `clima 140` topo direita + `moedas 3×2` `125×70` (`src/main.cpp:320`).
- **BTC cabe:** `pad 6`, `width-12`, fonte adaptativa `len>9→14, >7→16, else 20`, `LV_LABEL_LONG_SCROLL_CIRCULAR`.
- **Preview tempo real:** `onMoedaInput()` fetch `https://economia.awesomeapi.com.br/json/last/PAIR` direto no browser, mostra `R$ bid (pctChange%)` sem salvar. `Salvar moedas` → `POST /api/config` → `gNeedsRebuild` se `oldCnt≠newCnt`.

### Clima (todas as cidades do Brasil)

- `IBGE API` `servicodados.ibge.gov.br/api/v1/localidades/estados` → `municipios` (5570), filtro `cityInput` live, `select 140px`
- 27 capitais hardcode `CAPS[]` 1-clique `useCap()` + `geocoding-api.open-meteo.com` + `previewClima()` fetch `api.open-meteo.com/v1/forecast?latitude&longitude&current_weather=true` antes de salvar.

### Sistema/Rede

- `TELA`: `Brilho 10-255` range `input` → `POST /api/config {bright}` live `tft.setBrightness`, `Fuso`
- `APARÊNCIA`: `🌙 Noturno / ☀️ Claro` `setTheme()` transfere para ESP também
- `WIFI`: scan + `Salvar e Conectar` + `Reiniciar` + `OTA` card (ver OTA)
- `SOBRE`: `IP/SSID/lat,lon`

---

## Configurações do Display

### LovyanGFX (`src/LGFX_ESP32_8048S070.h:33-84`)

Timings validados após debug de tela lavada + varredura `top→bottom`:

```cpp
cfg.freq_write = 12000000;
cfg.hsync_polarity = 0; cfg.hsync_front_porch = 8; cfg.hsync_pulse_width = 2; cfg.hsync_back_porch = 43;
cfg.vsync_polarity = 0; cfg.vsync_front_porch = 8; cfg.vsync_pulse_width = 2; cfg.vsync_back_porch = 12;
cfg.pclk_idle_high = 1;
```

> Antes `210/30/16 + 22/13/10 @16MHz` causava varredura; `8/2/43` fixou. `Light PWM 44100Hz ch7` `src/LGFX:82` + `tft.setBrightness(180)` evita lavagem.

### LVGL (`src/lv_conf.h`)

- `LV_COLOR_DEPTH 16`, `LV_COLOR_16_SWAP 0`, `LV_TICK_CUSTOM 1 (millis)`, `LV_DISP_DEF_REFR_PERIOD 16` (~60Hz), `LV_DISP_ROT_MAX_BUF 10KB`, `LV_USE_PERF_MONITOR 0` (desativado para não piscar)
- Fonts habilitadas: `14/16/20/24/32/48` + `12` para moedas pequenas
- `my_disp_flush` `src/main.cpp:31` usa `tft.pushImage(area, (rgb565_t*)&color_p->full)` (não `writePixels` que inverte cores)
- Buffer `800×32` (`25600 px`) **single** `MALLOC_CAP_INTERNAL` `src/main.cpp:524` (`heap_caps_malloc`) — `240×40` causava `FPS` variando, `800×48 double PSRAM` causava tearing/piscando.

### Brilho

`gConfig.brightness` `180` padrão (`src/app_config.h:24`), slider web `10-255` live via `POST /api/config {bright}`. `110` com PWM baixo causava batimento → `180`.

### Tema Claro/Escuro (transfere para ESP)

`gConfig.display_light` `bool` `src/app_config.h:29` → `Preferences "dlight"`:

```cpp
bool light = gConfig.display_light;
lv_color_t colBg = light? 0xF1F5F9 : 0x070A12;
lv_color_t colCard = light? 0xFFFFFF : 0x0F1622;
lv_color_t colText = light? 0x0F172A : 0xF8FAFC;
// ... header/border/muted
```

`create_ui()` recria em `loop()` se `gNeedsRebuild` `src/main.cpp:641`. Web `toggleTheme()` `src/web_server.cpp:24` `setTheme(m, true)` faz `fetch POST {dlight}` + `localStorage`.

---

## OTA Update com GitHub Actions

### Visão

```
git tag v2.0.7 && git push origin v2.0.7
  → GitHub Action pio run → firmware-v2.0.7.bin → Release automática
  → ESP verifica release (API com fallback por redirect, tolerante a rate-limit 403)
  → se latest_code > FIRMWARE_VERSION_CODE → baixa .bin em task dedicada → Update → restart
  → auto-update ~30s após detectar nova versão + manual no WebServer
```

### Versão (`src/version.h`)

```cpp
#define FIRMWARE_VERSION "2.0.7"
#define FIRMWARE_VERSION_CODE 207 // 2.0.7 = 207
#define GITHUB_REPO "luanvieiramendes/ROBOBUILDERS"
#define GITHUB_API_LATEST "https://api.github.com/repos/luanvieiramendes/ROBOBUILDERS/releases/latest"
```

Incremente `FIRMWARE_VERSION` + `CODE` (`2.0.6→2.0.7 = 207`) a cada release. `CODE = maj*100+min*10+pat`.

> ⚠️ **Regra de ouro:** a tag do release (ex: `v2.0.7`) deve ser **sempre maior** que o `FIRMWARE_VERSION_CODE` rodando no ESP. Se forem iguais, o ESP entende `latest == current` e não atualiza (log `ja atualizado`).

### Updater (`src/ota_updater.h/.cpp`)

- `otaInit()` loga versão e **cria a task FreeRTOS `ota_task` com 32KB de stack** (core 0, prioridade 1).
- `otaCheck()` — chamado por timers (15s pós-boot + a cada 6h):
  1. Tenta `GET GITHUB_API_LATEST` com `User-Agent: ESP32-OTA` + `Accept: application/vnd.github+json`;
  2. Se `200`: extrai `tag_name` e o primeiro `assets[*].browser_download_url` terminado em `.bin`;
  3. Se `403` (rate-limit da API sem token): **fallback `checkViaRedirect()`** — `GET https://github.com/<repo>/releases/latest` sem seguir redirect (`HTTPC_DISABLE_FOLLOW_REDIRECTS`) e extrai a tag da `Location` (`/tag/vX.Y.Z`). Isso **não sofre rate-limit** da API;
  4. Se a API não forneceu URL do asset, monta a padrão: `https://github.com/<repo>/releases/download/<tag>/firmware-<tag>.bin`;
  5. Converte `tag → code` via `versionToCode()` e compara com `FIRMWARE_VERSION_CODE`;
  6. `latest > current` → `return true` (update disponível), senão `false` (`ja atualizado`).
- `otaUpdate()` é wrapper de `otaUpdateUrl(url)`: `HTTPClient.setFollowRedirects(FORCE)` `GET downloadUrl` → `Update.begin(len)` → `Update.write(buff 4KB)` com `gOta.progress` → `Update.end(true)` → `ESP.restart()`. Estados `OTA_CHECKING/UPDATING/SUCCESS/FAILED`.
- `otaRequestUpdate()` captura `downloadUrl` **no momento do agendamento** (cópia `sOtaUrl`) e sinaliza a task — assim um `otaCheck()` concorrente que limpa `gOta.downloadUrl` não quebra o download.
- `otaUpdateUrl()` roda na `ota_task` (background), com `esp_task_wdt_reset()` durante o download e `http.setTimeout(20000)`.

### GitHub Action (`.github/workflows/build-release.yml`)

```yaml
name: Build and Release Firmware
on:
  push:
    branches: [main]
    tags: ['v*.*.*']
  workflow_dispatch:
permissions:
  contents: write
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with: {python-version: '3.11'}
      - run: pip install platformio
      - run: pio run -e esp32-8048s070
      - run: cp .pio/build/esp32-8048s070/firmware.bin firmware-${{github.ref_name}}.bin && cp ... firmware-latest.bin
      - uses: actions/upload-artifact@v4
        with: {name: firmware-bin, path: firmware-*.bin}
      - if: startsWith(github.ref,'refs/tags/v')
        uses: softprops/action-gh-release@v2
        with:
          files: |
            firmware-*.bin
            firmware-latest.bin
          generate_release_notes: true
```

- Push em `main` só builda + `upload-artifact` (preview).
- `git tag v2.0.7` → `Release` com `firmware-v2.0.7.bin` + `firmware-latest.bin` que o `ESP` busca.

### WebServer OTA (auto + manual)

- Card `ATUALIZAÇÃO OTA` com `Versão atual / Última / Status` `otaBar` + `🔍 Verificar` `POST /api/ota/check` + `⬇️ Atualizar agora` `POST /api/ota/update`.
- Endpoints:
  - `GET /api/version` → `{current,latest,state,progress,error,url,dlight}`
  - `POST /api/ota/check` → `otaCheck()` → `{hasUpdate,latest,msg,state}`
  - `POST /api/ota/update` → `otaRequestUpdate()` → `{msg,ok}` — **responde na hora** e o download roda na `ota_task` em background (sem travar o webserver)
- Timers em `src/main.cpp`: `15s` pós-boot `otaCheck(true)`, `6h` periódico, e `30s` auto-update: se `latest > current` → `otaRequestUpdate()` (baixa sozinho).

### Como lançar OTA (fluxo com tag — sempre `add` → `commit` → `push`)

> **Regra:** `push` em `main` só gera `artifact` em `Actions` (preview `firmware-main.bin`). Para ir para `Releases` e o `ESP` atualizar via `OTA`, **tem que ser com `tag` `v*.*.*`**.

```bash
# 1. bump versão no firmware (tag deve ser > FIRMWARE_VERSION_CODE rodando no ESP)
# edite src/version.h:
#   #define FIRMWARE_VERSION "2.0.7"
#   #define FIRMWARE_VERSION_CODE 207  // 2.0.7 = 207

# 2. sempre add + commit + push (obrigatório)
git add src/version.h
git commit -m "chore: bump 2.0.7"
git push origin main

# 3. crie a tag que dispara o Release (mesma versão do passo 1)
git tag v2.0.7
git push origin v2.0.7   # <-- este push cria o Release

# 4. acompanhe: https://github.com/luanvieiramendes/ROBOBUILDERS/actions
#    → workflow "Build and Release Firmware" verde
#    → https://github.com/luanvieiramendes/ROBOBUILDERS/releases → firmware-v2.0.7.bin + firmware-latest.bin

# 5. ESP busca automaticamente (15s pós-boot, 6h, e auto-update ~30s)
#    ou manual: WebServer → Sistema → OTA → 🔍 Verificar → ⬇️ Atualizar agora → barra 0-100% → restart auto
#    Footer do display já mostra v2.0.7
```

**Dica:** se esquecer o `push` da tag, o `.bin` fica só em `Actions → Artifacts` (30 dias) e **não** vai para `Releases`, o `OTA` não encontra. Sempre faça `git push origin vX.Y.Z` após `git tag`.

Log de sucesso no Serial Monitor (115200):

```
[OTA] atual=2.0.6 (206) latest=2.0.7 (207)
[OTA] url=https://github.com/luanvieiramendes/ROBOBUILDERS/releases/download/v2.0.7/firmware-v2.0.7.bin
[OTA] Atualizacao disponivel: v2.0.7
[OTA] auto update disponivel, baixando em background...
[OTA] iniciando download https://github.com/.../firmware-v2.0.7.bin
[OTA] tamanho 1504336
[OTA] 56607/1504336 (3%)
...
[OTA] 1494303/1504336 (99%)
[OTA] sucesso, reiniciando...
→ após restart: [OTA] versao atual 2.0.7 (207) / [OTA] ja atualizado
```

---

## Troubleshooting OTA

Todo o conhecimento obtido em campo (com Monitor Serial 115200) — consulte os logs para identificar cada caso.

| Sintoma no Monitor | Causa | Fix |
|---|---|---|
| `Guru Meditation ... Stack canary watchpoint triggered (ota_task)` e reboot **no meio do download** | **Stack 8KB insuficiente** na task: `HTTPClient` + `WiFiClient` + buffer 4KB estouram a stack | Aumentar stack da task para **32KB** (`xTaskCreatePinnedToCore(..., 32768, ...)`) — `ota_updater.cpp:otaInit` |
| `reboot`/`rst:0xc` ao clicar "Atualizar agora", sem progresso | **Task WDT**: download rodando **dentro do handler do webserver** bloqueia a loopTask > 5s → watchdog reseta | Update em **task FreeRTOS dedicada**; o handler só agenda (`otaRequestUpdate()`) e responde imediatamente |
| `[OTA] sem URL, faca Verificar antes` (download nunca inicia) | **Race condition**: `otaCheck()` periódico (15s/6h) limpa `gOta.downloadUrl` enquanto a task ia baixar | Capturar a URL **no agendamento** (`otaRequestUpdate()` salva cópia `sOtaUrl`) e passá-la para `otaUpdateUrl(url)` |
| `API HTTP 403` + `"API rate limit exceeded"` | **Rate-limit da api.github.com** (60 req/h sem token, por IP) | Já tratado: `checkViaRedirect()` usa `github.com/<repo>/releases/latest` com redirect manual — **não sofre rate-limit**. Verificar funciona mesmo com 403 |
| `[OTA] download falha -1` / `SSL - Memory allocation failed` (WiFiClientSecure) | Falha transitória de **memória heap** (TLS) durante download (UI LVGL usa bastante RAM) | Retry automático (próximo timer 30s tenta de novo) — em produção acabou funcionando na 2ª tentativa |
| `[OTA] download HTTP 404` | URL do asset errada ou release sem o `.bin` (tag não pushada ou build falhou) | Confirmar `firmware-vX.Y.Z.bin` na Release; há fallback automático para `firmware-latest.bin` |
| `[OTA] ja atualizado` mesmo com release nova | **Tag/release igual ou menor** que o `FIRMWARE_VERSION_CODE` do ESP (ex: ESP em 207 e release v2.0.7) | Bump corretamente: a release precisa ser **maior** que a versão gravada no ESP; cada release exige `version.h` atualizado + tag `v*.*.*` nova |
| Release não aparece no GitHub (só artifact) | Tag **não foi pushada** — push em `main` só gera artifact | `git tag vX.Y.Z && git push origin vX.Y.Z` — o **push da tag** é o gatilho do `softprops/action-gh-release` |
| `fallback: redirect sem /tag/` | Link de release sem padrão `/tag/` na Location (release de volume/página) | Confirmar que a release é pública; funciona com releases normais `v*.*.*` |

### Decisões de arquitetura aprendidas

1. **Nunca rode o download OTA no handler do webserver** — bloqueia a loopTask e mata o Task WDT (5s). Sempre task dedicada.
2. **A task de update precisa de stack generosa (32KB)** — `HTTPClient` + `WiFiClient` + buffers explodem 8KB (stack canary panic).
3. **URL do download deve ser capturada no momento do agendamento** — checks periódicos limpam o estado global `gOta`.
4. **API do GitHub sem token tem rate-limit de 60 req/h por IP** — o fallback por redirect de `github.com/<repo>/releases/latest` é infinito e rápido.
5. **Comparação por código numérico** (`maj*100+min*10+pat`), nunca por string — `v2.0.10 > v2.0.9` não seria detectado como string.
6. **Cada release precisa de `version.h` bumpado + tag única** — senão o ESP entende que já está atualizado.

---

## API

| Método | Rota | Body | Resp |
|---|---|---|---|
| `GET` | `/` | — | HTML SPA |
| `GET` | `/api/config` | — | `{c1..c6,c1en..c6en,city,lat,lon,bright,tz,dint,wint,ssid,ip,dlight}` |
| `POST` | `/api/config` | `{c1..c6,c1en..,city,lat,lon,bright,tz,dint,wint,ssid,pass,dlight}` | `{msg}` + `gNeedsRebuild` se moedas/dlight mudaram |
| `GET` | `/api/data` | — | `{time,date,dolar,weatherTemp,weatherDesc,city,wifi,ip,uptime,heap,bright}` |
| `GET` | `/api/scan` | — | `[{ssid,rssi,encryption,channel}]` |
| `GET` | `/api/version` | — | `{current,latest,state,progress,error,url,dlight}` |
| `POST` | `/api/ota/check` | — | `{hasUpdate,latest,msg,state}` |
| `POST` | `/api/ota/update` | — | `{msg,ok}` + inicia OTA |
| `POST` | `/api/restart` | — | `{msg}` |

Exemplo `curl`:

```bash
curl http://192.168.1.57/api/config | jq
curl -X POST http://192.168.1.57/api/config -H "Content-Type: application/json" -d '{"c1":"USD-BRL","c1en":true,"city":"Rio de Janeiro","lat":-22.9,"lon":-43.17,"bright":200}'
```

---

## Reutilizando em Outros Projetos

Este sistema foi desenhado modular para copiar:

### 1. Copie os arquivos

```bash
cp src/LGFX_ESP32_8048S070.h src/lv_conf.h src/app_config.* src/web_server.* src/ota_updater.* src/version.h ~/NovoProjeto/src/
cp platformio.ini ~/NovoProjeto/ # ou mescle lib_deps
mkdir -p ~/NovoProjeto/.github/workflows
cp .github/workflows/build-release.yml ~/NovoProjeto/.github/workflows/
```

### 2. `platformio.ini` mínimo

```ini
[env:esp32-8048s070]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
board_build.arduino.memory_type = qio_opi
board_build.flash_mode = qio
build_flags = -DBOARD_HAS_PSRAM -DLV_CONF_INCLUDE_SIMPLE -DLV_CONF_SUPPRESS_DEFINE_CHECK -I./src
lib_deps = lovyan03/LovyanGFX@^1.1.15
           lvgl/lvgl@^8.3.11
           bblanchon/ArduinoJson@^7.0.4
```

### 3. `main.cpp` mínimo

```cpp
#include "LGFX_ESP32_8048S070.h"
#include "app_config.h"
#include "web_server.h"
#include "ota_updater.h"
#include "version.h"
LGFX tft;
void my_flush(...) { tft.pushImage(...); }
void setup(){
  loadConfig();
  lv_init(); tft.init(); tft.setBrightness(gConfig.brightness);
  // buffer 800*32 single IRAM
  // ... lv_disp + lv_indev + create_ui()
  WiFi.begin(gConfig.wifi_ssid, gConfig.wifi_pass);
  webServerInit();
  otaInit();
}
void loop(){ lv_timer_handler(); webServerLoop(); }
```

### 4. Troque `GITHUB_REPO` em `src/version.h` e `src/ota_updater.cpp` para seu repo.

### 5. Customizar display

Edite `src/LGFX_ESP32_8048S070.h:33` pinagem e `src/main.cpp:96` `create_ui()` cores/layout. Para novo painel (ex `3.5" 320×480`), só troque `Bus_RGB` config e `hor_res/ver_res`.

---

## Troubleshooting Display

| Sintoma | Causa | Fix |
|---|---|---|
| **Tela lavada / branca estourada** | `tft.setBrightness(255)` ou `freq 5000` | `44100Hz` `src/LGFX:82` + `setBrightness(180)` `src/main.cpp:519` |
| **Varredura top→bottom / flicker** | timings `210/30/16` do `Arduino_GFX` no `Lovyan` | `8/2/43 + 8/2/12 @12MHz pclk_idle_high=1` `src/LGFX:33` |
| **Cores invertidas (azul vira vermelho)** | `my_disp_flush` `writePixels` inverte | `pushImage` `src/main.cpp:31` + `LV_COLOR_16_SWAP 0` |
| **FPS variando 20-66** | buffer `240*40` pequeno | `800*32 single IRAM` `src/main.cpp:523` + `LV_DISP_DEF_REFR_PERIOD 16` |
| **Piscando rápido** | `double PSRAM` + `PERF_MONITOR` + `PWM 5000` | `single IRAM` + `PERF_MONITOR 0` + `44100Hz` |
| **BTC `R$ --` cortado** | `pad 20` + `font 32` em `125px` | `pad 6` + fonte adaptativa `len>9→14` `src/main.cpp:430` |
| **WiFi não conecta** | `SSID` salvo `iPhone Antony` offline | Fallback `ROBOBUILDERS` `src/main.cpp:559` ou `AP 192.168.4.1` → `Buscar redes` |

---

## Roadmap

- [ ] Chart histórico câmbio (awesomeAPI `json/daily`)
- [ ] Touch para trocar moeda direto no display
- [ ] LittleFS para HTML (hoje `PROGMEM` ~45KB)
- [ ] MQTT + Home Assistant

---

## Licença

MIT — use livremente em outros projetos. Mantenha créditos.

> **Autor:** Antony — `luanvieiramendes/ROBOBUILDERS` — `ESP32-8048S070` `S3` `LovyanGFX` `LVGL` `ArduinoJson`

