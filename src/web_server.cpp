#include "web_server.h"
#include "app_config.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include "LGFX_ESP32_8048S070.h"

extern LGFX tft;
extern String dolarValue;
extern String weatherTemp;
extern String weatherDesc;
extern String weatherCity;

WebServer webServer(80);

// HTML da pagina de config - single file, sem LittleFS
static const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Painel Financeiro - Config</title>
<style>
:root{--bg:#0B0B0E;--card:#12121A;--accent:#22D3EE;--yellow:#FFB300;--green:#00E676;--text:#F8FAFC;--muted:#7A8699;--border:#1E2A3A}
*{box-sizing:border-box;font-family:Inter,system-ui,sans-serif}
body{margin:0;background:linear-gradient(180deg,#070A12,#0E1420);color:var(--text);padding:20px}
h1{font-size:22px;margin:0 0 4px}
.sub{color:var(--muted);font-size:13px;margin-bottom:20px}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(340px,1fr));gap:16px}
.card{background:var(--card);border:1px solid var(--border);border-radius:16px;padding:16px;position:relative;overflow:hidden}
.card::before{content:"";position:absolute;top:0;left:0;right:0;height:4px}
.card.clima::before{background:var(--yellow)}
.card.moeda::before{background:var(--green)}
.card.sistema::before{background:var(--accent)}
.card h2{font-size:13px;letter-spacing:2px;color:var(--accent);margin:0 0 12px}
.card.moeda h2{color:var(--green)}
.card.clima h2{color:var(--yellow)}
label{font-size:12px;color:var(--muted);display:block;margin:8px 0 4px}
input,select{width:100%;padding:10px 12px;border-radius:10px;border:1px solid var(--border);background:#0F1622;color:var(--text);font-size:14px}
.row{display:flex;gap:10px}
.row>div{flex:1}
.check{display:flex;align-items:center;gap:8px;margin:6px 0}
.check input{width:auto}
button{background:var(--accent);color:#000;border:0;padding:12px 16px;border-radius:12px;font-weight:700;cursor:pointer;width:100%;margin-top:12px}
button.green{background:var(--green)}
button.dark{background:#1E2A3A;color:var(--text)}
.badge{display:inline-block;padding:4px 8px;border-radius:20px;font-size:11px;font-weight:700}
.badge.ok{background:rgba(0,230,118,0.15);color:var(--green);border:1px solid var(--green)}
.badge.no{background:rgba(255,82,82,0.15);color:#FF5252;border:1px solid #FF5252}
.kv{display:flex;justify-content:space-between;padding:6px 0;border-bottom:1px solid #1E2A3A;font-size:13px}
.kv span{color:var(--muted)}
pre{background:#070A12;padding:12px;border-radius:10px;overflow:auto;font-size:12px;border:1px solid var(--border)}
</style>
</head>
<body>
<h1>PAINEL FINANCEIRO <span id="ip" style="float:right;font-size:12px;color:var(--muted)"></span></h1>
<div class="sub">Configure moedas, clima e sistema. IP mostrado no display.</div>

<div class="grid">
 <div class="card sistema">
  <h2>DADOS AO VIVO</h2>
  <div id="live"></div>
  <button class="dark" onclick="loadData()">Atualizar agora</button>
 </div>

 <div class="card moeda">
  <h2>MOEDAS (AwesomeAPI)</h2>
  <div class="check"><input type="checkbox" id="c1en"><label for="c1en" style="margin:0">Ativar 1</label><input id="c1" placeholder="USD-BRL" style="margin-left:auto;flex:1"></div>
  <div class="check"><input type="checkbox" id="c2en"><label for="c2en" style="margin:0">Ativar 2</label><input id="c2" placeholder="EUR-BRL" style="margin-left:auto;flex:1"></div>
  <div class="check"><input type="checkbox" id="c3en"><label for="c3en" style="margin:0">Ativar 3</label><input id="c3" placeholder="BTC-BRL" style="margin-left:auto;flex:1"></div>
  <label>Dicas: USD-BRL, EUR-BRL, BTC-BRL, GBP-BRL, JPY-BRL, ETH-BRL</label>
  <label>Intervalo cotações (s) <input id="dint" type="number" min="30" max="3600"></label>
 </div>

 <div class="card clima">
  <h2>CLIMA (Open-Meteo)</h2>
  <label>Cidade</label><input id="city" placeholder="Sao Paulo">
  <div class="row"><div><label>Latitude</label><input id="lat" type="number" step="0.0001"></div><div><label>Longitude</label><input id="lon" type="number" step="0.0001"></div></div>
  <button class="dark" onclick="buscarCidade()">Buscar lat/lon pela cidade</button>
  <label>Intervalo clima (s) <input id="wint" type="number" min="60" max="3600"></label>
 </div>

 <div class="card">
  <h2>SISTEMA & TELA</h2>
  <label>Brilho (0-255) <input id="bright" type="range" min="10" max="255" oninput="document.getElementById('bv').innerText=this.value"><span id="bv"></span></label>
  <label>Fuso horário</label>
  <select id="tz">
   <option value="-5">-5 Acre</option><option value="-4">-4 Manaus</option><option value="-3" selected>-3 Brasília</option><option value="-2">-2 Fernando</option>
  </select>
  <label>WiFi SSID <input id="ssid"></label>
  <label>WiFi Senha <input id="pass" type="password"></label>
  <div style="font-size:11px;color:#FF5252;margin-top:6px">Trocar WiFi reinicia e tenta conectar 20s; se falhar volta ao anterior.</div>
 </div>
</div>

<button class="green" onclick="save()">💾 Salvar e Aplicar</button>
<button class="dark" onclick="restart()">🔄 Reiniciar ESP</button>
<pre id="log"></pre>

<script>
let logEl=document.getElementById('log');
function log(m){logEl.textContent=m+"\n"+logEl.textContent}
async function loadConfig(){
 let r=await fetch('/api/config'); let j=await r.json();
 document.getElementById('c1').value=j.c1; document.getElementById('c2').value=j.c2; document.getElementById('c3').value=j.c3;
 document.getElementById('c1en').checked=j.c1en; document.getElementById('c2en').checked=j.c2en; document.getElementById('c3en').checked=j.c3en;
 document.getElementById('city').value=j.city; document.getElementById('lat').value=j.lat; document.getElementById('lon').value=j.lon;
 document.getElementById('bright').value=j.bright; document.getElementById('bv').innerText=j.bright;
 document.getElementById('tz').value=j.tz; document.getElementById('dint').value=j.dint; document.getElementById('wint').value=j.wint;
 document.getElementById('ssid').value=j.ssid; document.getElementById('ip').innerText=j.ip;
 log('Config carregado IP '+j.ip)
}
async function loadData(){
 let r=await fetch('/api/data'); let j=await r.json();
 document.getElementById('live').innerHTML=`
  <div class="kv"><span>Hora</span><b>${j.time} ${j.date}</b></div>
  <div class="kv"><span>Câmbio 1</span><b>${j.dolar}</b></div>
  <div class="kv"><span>Clima</span><b>${j.weatherTemp} ${j.weatherDesc} (${j.city})</b></div>
  <div class="kv"><span>WiFi</span><span class="badge ${j.wifi=='Conectado'?'ok':'no'}">${j.wifi} ${j.ip}</span></div>
  <div class="kv"><span>Uptime</span><b>${j.uptime}s</b></div>
  <div class="kv"><span>Heap</span><b>${j.heap} bytes</b></div>
  <div class="kv"><span>Brilho</span><b>${j.bright}</b></div>
 `;
}
async function buscarCidade(){
 let city=document.getElementById('city').value;
 if(!city){alert('Digite a cidade');return}
 log('Buscando '+city+'...')
 let r=await fetch('https://geocoding-api.open-meteo.com/v1/search?name='+encodeURIComponent(city)+'&count=1&language=pt&format=json');
 let j=await r.json();
 if(j.results && j.results[0]){ document.getElementById('lat').value=j.results[0].latitude; document.getElementById('lon').value=j.results[0].longitude; document.getElementById('city').value=j.results[0].name; log('Encontrado: '+j.results[0].name+' '+j.results[0].latitude+','+j.results[0].longitude)}
 else log('Cidade não encontrada')
}
async function save(){
 let body={
  c1:document.getElementById('c1').value, c2:document.getElementById('c2').value, c3:document.getElementById('c3').value,
  c1en:document.getElementById('c1en').checked, c2en:document.getElementById('c2en').checked, c3en:document.getElementById('c3en').checked,
  city:document.getElementById('city').value, lat:parseFloat(document.getElementById('lat').value), lon:parseFloat(document.getElementById('lon').value),
  bright:parseInt(document.getElementById('bright').value), tz:parseInt(document.getElementById('tz').value),
  dint:parseInt(document.getElementById('dint').value), wint:parseInt(document.getElementById('wint').value),
  ssid:document.getElementById('ssid').value, pass:document.getElementById('pass').value
 };
 let r=await fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});
 let j=await r.json(); log(j.msg); setTimeout(loadConfig, 800)
}
async function restart(){ if(confirm('Reiniciar?')) await fetch('/api/restart',{method:'POST'}) }
loadConfig(); loadData(); setInterval(loadData, 5000);
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  webServer.send_P(200, "text/html", HTML_PAGE);
}

void handleGetConfig() {
  JsonDocument doc;
  doc["c1"] = gConfig.currency_1;
  doc["c2"] = gConfig.currency_2;
  doc["c3"] = gConfig.currency_3;
  doc["c1en"] = gConfig.curr1_enabled;
  doc["c2en"] = gConfig.curr2_enabled;
  doc["c3en"] = gConfig.curr3_enabled;
  doc["city"] = gConfig.city;
  doc["lat"] = gConfig.lat;
  doc["lon"] = gConfig.lon;
  doc["bright"] = gConfig.brightness;
  doc["tz"] = gConfig.tz_offset;
  doc["dint"] = gConfig.dolar_interval;
  doc["wint"] = gConfig.weather_interval;
  doc["ssid"] = gConfig.wifi_ssid;
  doc["ip"] = WiFi.localIP().toString();
  String out;
  serializeJson(doc, out);
  webServer.send(200, "application/json", out);
}

void handlePostConfig() {
  if (!webServer.hasArg("plain")) {
    webServer.send(400, "application/json", "{\"msg\":\"sem body\"}");
    return;
  }
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, webServer.arg("plain"));
  if (err) {
    webServer.send(400, "application/json", "{\"msg\":\"json invalido\"}");
    return;
  }
  // moeda
  if (doc.containsKey("c1")) strlcpy(gConfig.currency_1, doc["c1"], sizeof(gConfig.currency_1));
  if (doc.containsKey("c2")) strlcpy(gConfig.currency_2, doc["c2"], sizeof(gConfig.currency_2));
  if (doc.containsKey("c3")) strlcpy(gConfig.currency_3, doc["c3"], sizeof(gConfig.currency_3));
  gConfig.curr1_enabled = doc["c1en"] | gConfig.curr1_enabled;
  gConfig.curr2_enabled = doc["c2en"] | gConfig.curr2_enabled;
  gConfig.curr3_enabled = doc["c3en"] | gConfig.curr3_enabled;
  if (doc.containsKey("city")) strlcpy(gConfig.city, doc["city"], sizeof(gConfig.city));
  if (doc.containsKey("lat")) gConfig.lat = doc["lat"];
  if (doc.containsKey("lon")) gConfig.lon = doc["lon"];
  if (doc.containsKey("bright")) {
    gConfig.brightness = doc["bright"];
    tft.setBrightness(gConfig.brightness);
  }
  if (doc.containsKey("tz")) {
    gConfig.tz_offset = doc["tz"];
    configTime(gConfig.tz_offset * 3600, 0, "pool.ntp.org", "time.nist.gov");
  }
  if (doc.containsKey("dint")) gConfig.dolar_interval = doc["dint"];
  if (doc.containsKey("wint")) gConfig.weather_interval = doc["wint"];
  bool wifiChanged = false;
  if (doc.containsKey("ssid") && doc["ssid"].as<String>().length()>0) {
    String ns = doc["ssid"]; String np = doc["pass"] | "";
    if (ns != gConfig.wifi_ssid || np.length()>0) {
      ns.toCharArray(gConfig.wifi_ssid, sizeof(gConfig.wifi_ssid));
      if (np.length()>0) np.toCharArray(gConfig.wifi_pass, sizeof(gConfig.wifi_pass));
      wifiChanged = true;
    }
  }
  saveConfig();
  if (wifiChanged) {
    webServer.send(200, "application/json", "{\"msg\":\"WiFi alterado, reconectando...\"}");
    delay(500);
    WiFi.begin(gConfig.wifi_ssid, gConfig.wifi_pass);
  } else {
    webServer.send(200, "application/json", "{\"msg\":\"Salvo!\"}");
  }
}

void handleGetData() {
  JsonDocument doc;
  // pega hora local
  struct tm ti;
  char timeStr[16] = "--:--:--";
  char dateStr[32] = "";
  if (getLocalTime(&ti)) {
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", ti.tm_hour, ti.tm_min, ti.tm_sec);
    snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", ti.tm_mday, ti.tm_mon+1, ti.tm_year+1900);
  }
  doc["time"] = timeStr;
  doc["date"] = dateStr;
  doc["dolar"] = dolarValue;
  doc["weatherTemp"] = weatherTemp;
  doc["weatherDesc"] = weatherDesc;
  doc["city"] = weatherCity;
  doc["wifi"] = WiFi.status()==WL_CONNECTED ? "Conectado" : "Desconectado";
  doc["ip"] = WiFi.localIP().toString();
  doc["uptime"] = millis()/1000;
  doc["heap"] = ESP.getFreeHeap();
  doc["bright"] = gConfig.brightness;
  String out; serializeJson(doc, out);
  webServer.send(200, "application/json", out);
}

void handleRestart() {
  webServer.send(200, "application/json", "{\"msg\":\"reiniciando...\"}");
  delay(500);
  ESP.restart();
}

void handleNotFound() {
  webServer.send(404, "text/plain", "Nao encontrado");
}

void webServerInit() {
  webServer.on("/", HTTP_GET, handleRoot);
  webServer.on("/api/config", HTTP_GET, handleGetConfig);
  webServer.on("/api/config", HTTP_POST, handlePostConfig);
  webServer.on("/api/data", HTTP_GET, handleGetData);
  webServer.on("/api/restart", HTTP_POST, handleRestart);
  webServer.onNotFound(handleNotFound);
  webServer.begin();
  Serial.println("[Web] http://"+WiFi.localIP().toString()+"/");
}

void webServerLoop() {
  webServer.handleClient();
}
