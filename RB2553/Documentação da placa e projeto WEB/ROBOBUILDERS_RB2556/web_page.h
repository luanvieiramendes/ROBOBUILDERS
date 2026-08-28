#pragma once

// ====================================================================
// ESP-32_RELAY_30A_X8_V1.2 - PAINEL DE CONTROLE E DIAGNÓSTICO
// 8 Relés de Potência 30A | LED Onboard: GPIO 5 | Botão Boot: GPIO 0 (IO0)
// ====================================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>ESP-32 Relay 30A X8 V1.2 - Painel de Controle</title>
  <style>
    :root {
      --bg: #0b0f19;
      --card-bg: #131b2e;
      --card-border: #1e293b;
      --text-main: #f8fafc;
      --text-muted: #94a3b8;
      --primary: #0ea5e9;
      --primary-hover: #0284c7;
      --green: #10b981;
      --green-hover: #059669;
      --red: #f43f5e;
      --red-hover: #e11d48;
      --yellow: #f59e0b;
      --relay-active-border: #10b981;
      --relay-active-bg: rgba(16, 185, 129, 0.12);
      --radius: 12px;
      --shadow-sm: 0 2px 4px rgba(0,0,0,0.3);
      --shadow-md: 0 8px 16px rgba(0,0,0,0.4);
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
    body { background: var(--bg); color: var(--text-main); padding: 14px; max-width: 1000px; margin: 0 auto; line-height: 1.4; }

    header { background: var(--card-bg); border: 1px solid var(--card-border); border-radius: var(--radius); padding: 18px 16px; margin-bottom: 18px; text-align: center; box-shadow: var(--shadow-sm); }
    h1 { font-size: 1.45rem; color: var(--primary); margin-bottom: 4px; font-weight: 700; }
    p.sub { font-size: 0.88rem; color: var(--text-muted); }
    .badge-bar { display: flex; flex-wrap: wrap; justify-content: center; gap: 8px; margin-top: 12px; }
    .badge { background: #1e293b; border: 1px solid #334155; padding: 5px 12px; border-radius: 20px; font-size: 0.78rem; color: #cbd5e1; font-weight: 500; }
    .badge.live { border-color: #10b981; background: rgba(16, 185, 129, 0.15); color: #34d399; font-weight: 600; }

    .section-title { font-size: 1.15rem; color: #f1f5f9; margin: 24px 0 12px 4px; font-weight: 700; display: flex; align-items: center; justify-content: space-between; flex-wrap: wrap; gap: 8px; }
    .bulk-bar { display: flex; gap: 8px; margin-bottom: 14px; flex-wrap: wrap; }

    /* GRID DOS 8 RELÉS */
    .relay-grid { display: grid; grid-template-columns: 1fr; gap: 12px; margin-bottom: 20px; }
    @media (min-width: 540px) { .relay-grid { grid-template-columns: repeat(2, 1fr); } }
    @media (min-width: 860px) { .relay-grid { grid-template-columns: repeat(4, 1fr); } }

    .relay-card { background: var(--card-bg); border: 2px solid var(--card-border); border-radius: var(--radius); padding: 16px 14px; text-align: center; box-shadow: var(--shadow-sm); transition: all 0.2s ease; display: flex; flex-direction: column; justify-content: space-between; }
    .relay-card.active { border-color: var(--relay-active-border); background: var(--relay-active-bg); box-shadow: 0 0 14px rgba(16, 185, 129, 0.25); }
    .relay-head { display: flex; justify-content: space-between; align-items: center; margin-bottom: 6px; }
    .relay-ch { font-weight: 700; font-size: 1.05rem; color: #f8fafc; }
    .relay-chip { background: #1e293b; border: 1px solid #334155; font-size: 0.72rem; padding: 2px 7px; border-radius: 6px; color: #94a3b8; font-weight: 600; }
    .relay-desc { font-size: 0.78rem; color: var(--text-muted); margin-bottom: 10px; text-align: left; }
    
    .status-pill { display: inline-block; width: 100%; padding: 6px 10px; border-radius: 8px; font-weight: 700; font-size: 0.82rem; margin-bottom: 12px; letter-spacing: 0.3px; text-align: center; }
    .status-off { background: #1e293b; color: #94a3b8; border: 1px solid #334155; }
    .status-on { background: var(--green); color: #ffffff; }
    .status-led-on { background: var(--primary); color: #ffffff; }
    .status-pressed { background: #854d0e; color: #fef08a; border: 1px solid #facc15; }

    .btn-group { display: flex; gap: 6px; justify-content: center; }
    .btn { cursor: pointer; border: none; padding: 10px 14px; border-radius: 8px; font-weight: 600; font-size: 0.86rem; transition: background 0.15s, transform 0.1s; display: inline-flex; align-items: center; justify-content: center; gap: 6px; min-height: 44px; user-select: none; flex: 1; }
    .btn:active { transform: scale(0.97); }
    .btn-primary { background: var(--primary); color: #fff; }
    .btn-primary:hover { background: var(--primary-hover); }
    .btn-green { background: var(--green); color: #fff; }
    .btn-green:hover { background: var(--green-hover); }
    .btn-red { background: var(--red); color: #fff; }
    .btn-red:hover { background: var(--red-hover); }
    .btn-secondary { background: #1e293b; color: #cbd5e1; border: 1px solid #334155; }
    .btn-secondary:hover { background: #334155; }
    .btn-pulse { flex: 0 0 74px; font-size: 0.78rem; padding: 6px; }

    /* CARDS ONBOARD */
    .hero-grid { display: grid; grid-template-columns: 1fr; gap: 12px; margin-bottom: 20px; }
    @media (min-width: 600px) { .hero-grid { grid-template-columns: repeat(2, 1fr); } }
    .hero-card { background: var(--card-bg); border: 2px solid var(--card-border); border-radius: var(--radius); padding: 16px; text-align: center; box-shadow: var(--shadow-sm); transition: all 0.2s; }
    .hero-card.active { border-color: var(--green); background: rgba(16, 185, 129, 0.08); }
    .hero-card.led-active { border-color: var(--primary); background: rgba(14, 165, 233, 0.08); }
    .hero-card h3 { font-size: 1.05rem; margin-bottom: 4px; color: #f8fafc; }
    .hero-card .pin-tag { font-size: 0.8rem; color: var(--text-muted); margin-bottom: 10px; display: block; }

    /* GRADE DE GPIOS DE EXPANSÃO */
    .gpio-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(150px, 1fr)); gap: 10px; }
    .gpio-card { background: var(--card-bg); border: 1px solid var(--card-border); border-radius: var(--radius); padding: 12px; box-shadow: var(--shadow-sm); transition: all 0.15s; display: flex; flex-direction: column; justify-content: space-between; }
    .gpio-card.output-on { border-color: var(--green); background: rgba(16, 185, 129, 0.08); }
    .gpio-card.input-mode { border-style: dashed; background: #0e1626; }
    
    .gpio-head { display: flex; justify-content: space-between; align-items: center; margin-bottom: 6px; }
    .gpio-name { font-weight: 700; font-size: 0.9rem; color: #f1f5f9; }
    .gear-btn { background: #1e293b; border: 1px solid #334155; border-radius: 6px; width: 32px; height: 32px; display: inline-flex; align-items: center; justify-content: center; cursor: pointer; color: #94a3b8; font-size: 0.95rem; transition: background 0.15s; }
    .gear-btn:hover { background: #334155; color: #f8fafc; }

    .gpio-mode-tag { font-size: 0.7rem; padding: 2px 6px; border-radius: 4px; font-weight: 600; display: inline-block; margin-bottom: 8px; width: fit-content; }
    .mode-output { background: rgba(14, 165, 233, 0.2); color: #38bdf8; }
    .mode-input { background: rgba(245, 158, 11, 0.2); color: #fbbf24; }

    .btn-toggle { width: 100%; min-height: 40px; padding: 8px 4px; border-radius: 6px; border: 1px solid #334155; font-weight: 700; font-size: 0.8rem; cursor: pointer; transition: all 0.15s; }
    .btn-toggle.state-0 { background: #1e293b; color: #94a3b8; }
    .btn-toggle.state-1 { background: var(--green); color: #ffffff; border-color: var(--green); }
    
    .input-badge { padding: 8px 4px; border-radius: 6px; font-weight: 700; font-size: 0.8rem; text-align: center; display: block; border: 1px solid #334155; min-height: 40px; display: flex; align-items: center; justify-content: center; }
    .input-0 { background: #1e293b; color: #94a3b8; }
    .input-1 { background: rgba(14, 165, 233, 0.25); color: #38bdf8; border-color: rgba(14, 165, 233, 0.5); }

    /* MODAL */
    .modal-overlay { position: fixed; inset: 0; background: rgba(0, 0, 0, 0.75); display: none; align-items: center; justify-content: center; z-index: 999; padding: 14px; }
    .modal { background: #131b2e; border: 1px solid #334155; border-radius: var(--radius); padding: 20px; max-width: 360px; width: 100%; box-shadow: var(--shadow-md); color: var(--text-main); }
    .modal h3 { font-size: 1.15rem; color: #f8fafc; margin-bottom: 12px; }
    .modal p { font-size: 0.85rem; color: var(--text-muted); margin-bottom: 16px; }
    .radio-option { display: flex; align-items: center; gap: 10px; padding: 10px; border: 1px solid #1e293b; border-radius: 8px; margin-bottom: 10px; cursor: pointer; font-weight: 500; font-size: 0.9rem; background: #0b0f19; }
    .radio-option:hover { background: #1a233a; }
    .modal-actions { display: flex; justify-content: flex-end; gap: 8px; margin-top: 18px; }

    footer { text-align: center; font-size: 0.8rem; color: var(--text-muted); margin-top: 28px; padding: 16px; border-top: 1px solid var(--card-border); }
  </style>
</head>
<body>

  <header>
    <h1>ESP-32 RELAY 30A X8 V1.2</h1>
    <p class="sub">Painel de Diagnóstico e Controle de 8 Canais de Alta Potência (30A)</p>
    <div class="badge-bar">
      <span class="badge live">● Conectado (192.168.4.1)</span>
      <span class="badge">Wi-Fi: ESP32-RELAY-30A-8CH</span>
      <span class="badge" id="uptimeBadge">Uptime: 0s</span>
    </div>
  </header>

  <!-- 1. OS 8 RELÉS DE POTÊNCIA -->
  <div class="section-title">
    <span>⚡ 1. Relés de Potência 30A (8 Canais)</span>
  </div>
  
  <div class="bulk-bar">
    <button class="btn btn-green" onclick="setRelays(1)">⚡ Ligar Todos os Relés</button>
    <button class="btn btn-red" onclick="setRelays(0)">⛔ Desligar Todos os Relés</button>
  </div>

  <div class="relay-grid" id="relayContainer">
    <!-- Gerado dinamicamente via JS -->
  </div>

  <!-- 2. RECURSOS ONBOARD -->
  <div class="section-title">
    <span>🔧 2. Recursos Onboard (LED & Botão)</span>
  </div>
  
  <div class="hero-grid">
    <!-- LED ONBOARD (GPIO 5) -->
    <div class="hero-card" id="cardLed">
      <h3>LED Onboard</h3>
      <span class="pin-tag">Pino Físico: <b>GPIO 5</b></span>
      <div id="pillLed" class="status-pill status-off">DESLIGADO</div>
      <div class="btn-group">
        <button class="btn btn-primary" onclick="togglePin(5)">Ligar / Desligar</button>
        <button class="btn btn-secondary" onclick="pulsePin(5, 500)">Piscar (500ms)</button>
      </div>
    </div>

    <!-- BOTÃO BOOT / IO0 (GPIO 0) -->
    <div class="hero-card" id="cardBoot">
      <h3>Botão Boot / IO0</h3>
      <span class="pin-tag">Pino Físico: <b>GPIO 0</b></span>
      <div id="pillBoot" class="status-pill status-off">SOLTO (HIGH)</div>
      <p style="font-size:0.78rem; color:var(--text-muted); margin-top:8px;">Pressione o botão IO0/BOOT na placa para ver a leitura em tempo real.</p>
    </div>
  </div>

  <!-- 3. BARRAMENTO DE EXPANSÃO / DEMAIS GPIOS -->
  <div class="section-title">
    <span>🔌 3. Barramento de GPIOs & Expansão</span>
  </div>
  
  <div class="bulk-bar">
    <button class="btn btn-secondary" onclick="setAll(1)">Ligar Todas as Saídas</button>
    <button class="btn btn-secondary" onclick="setAll(0)">Desligar Todas</button>
  </div>
  
  <div class="gpio-grid" id="gpioContainer"></div>

  <!-- MODAL DE CONFIGURAÇÃO DE MODO -->
  <div class="modal-overlay" id="modalConfig">
    <div class="modal">
      <h3 id="modalTitle">Configurar GPIO</h3>
      <p>Selecione a função deste pino:</p>
      
      <label class="radio-option">
        <input type="radio" name="pinModeChoice" value="output" id="radioOutput">
        <div>
          <b>OUTPUT (Saída Digital)</b>
          <div style="font-size:0.75rem; color:#94a3b8;">Permite acionar cargas e atuadores</div>
        </div>
      </label>

      <label class="radio-option">
        <input type="radio" name="pinModeChoice" value="input" id="radioInput">
        <div>
          <b>INPUT (Entrada Digital com Pull-Up)</b>
          <div style="font-size:0.75rem; color:#94a3b8;">Lê sinais de botões e sensores (HIGH/LOW)</div>
        </div>
      </label>

      <div class="modal-actions">
        <button class="btn btn-secondary" onclick="closeModal()">Cancelar</button>
        <button class="btn btn-primary" onclick="savePinMode()">Salvar</button>
      </div>
    </div>
  </div>

  <footer>
    ESP-32_RELAY_30A_X8_V1.2 • Firmware de Controle e Diagnóstico Industrial
  </footer>

  <script>
    // 8 Canais de Relé (Pinos Oficiais da Placa ESP-32_RELAY_30A_X8_V1.2)
    const relayList = [
      { ch: 1, pin: 32 },
      { ch: 2, pin: 33 },
      { ch: 3, pin: 25 },
      { ch: 4, pin: 26 },
      { ch: 5, pin: 27 },
      { ch: 6, pin: 14 },
      { ch: 7, pin: 12 },
      { ch: 8, pin: 13 }
    ];

    // Demais GPIOs disponíveis para expansão e teste (LED Onboard é GPIO 5)
    const extraGpioList = [2, 4, 15, 16, 17, 18, 19, 21, 22, 23, 34, 35];

    let relayStates = {};
    relayList.forEach(function(r) { relayStates[r.pin] = 0; });

    let pinConfigs = {};
    let currentConfigPin = null;

    extraGpioList.forEach(function(p) {
      var isInputOnly = (p === 34 || p === 35);
      pinConfigs[p] = { mode: isInputOnly ? 'input' : 'output', val: 0 };
    });

    function renderRelays() {
      var container = document.getElementById('relayContainer');
      if (!container) return;
      container.innerHTML = '';

      relayList.forEach(function(r) {
        var state = relayStates[r.pin] || 0;
        var card = document.createElement('div');
        card.className = 'relay-card' + (state === 1 ? ' active' : '');
        card.id = 'relay_card_' + r.pin;

        card.innerHTML = 
          '<div>' +
            '<div class="relay-head">' +
              '<span class="relay-ch">Canal ' + r.ch + '</span>' +
              '<span class="relay-chip">GPIO ' + r.pin + '</span>' +
            '</div>' +
            '<div class="relay-desc">Relé de Potência 30A</div>' +
            '<div class="status-pill ' + (state === 1 ? 'status-on' : 'status-off') + '" id="relay_pill_' + r.pin + '">' +
              (state === 1 ? 'LIGADO (FECHADO)' : 'DESLIGADO (ABERTO)') +
            '</div>' +
          '</div>' +
          '<div class="btn-group">' +
            '<button class="btn ' + (state === 1 ? 'btn-red' : 'btn-green') + '" id="relay_btn_' + r.pin + '" onclick="togglePin(' + r.pin + ')">' +
              (state === 1 ? 'Desligar' : 'Ligar Relé') +
            '</button>' +
            '<button class="btn btn-secondary btn-pulse" onclick="pulsePin(' + r.pin + ', 1000)" title="Pulso temporizado de 1s">Pulso 1s</button>' +
          '</div>';

        container.appendChild(card);
      });
    }

    function renderExtraGPIOs() {
      var container = document.getElementById('gpioContainer');
      if (!container) return;
      container.innerHTML = '';

      extraGpioList.forEach(function(p) {
        var cfg = pinConfigs[p];
        var isInputOnly = (p === 34 || p === 35);
        var card = document.createElement('div');
        card.className = 'gpio-card' + (cfg.mode === 'output' && cfg.val === 1 ? ' output-on' : '') + (cfg.mode === 'input' ? ' input-mode' : '');
        card.id = 'card_g_' + p;

        var actionHtml = '';
        if (cfg.mode === 'output') {
          actionHtml = '<button class="btn-toggle state-' + cfg.val + '" id="btoggle_' + p + '" onclick="togglePin(' + p + ')">' +
            (cfg.val === 1 ? 'LIGADO (HIGH)' : 'DESLIGADO (LOW)') +
            '</button>';
        } else {
          actionHtml = '<span class="input-badge input-' + cfg.val + '" id="inbadge_' + p + '">' +
            'LEITURA: ' + (cfg.val === 1 ? 'HIGH (1)' : 'LOW (0)') +
            '</span>';
        }

        var gearHtml = isInputOnly ? '' : '<button class="gear-btn" title="Configurar Modo" onclick="openModal(' + p + ')">⚙️</button>';

        card.innerHTML =
          '<div>' +
            '<div class="gpio-head">' +
              '<span class="gpio-name">GPIO ' + p + '</span>' +
              gearHtml +
            '</div>' +
            '<span class="gpio-mode-tag ' + (cfg.mode === 'output' ? 'mode-output' : 'mode-input') + '" id="modetag_' + p + '">' +
              cfg.mode.toUpperCase() +
            '</span>' +
          '</div>' +
          '<div style="margin-top:4px;">' +
            actionHtml +
          '</div>';

        container.appendChild(card);
      });
    }

    function openModal(p) {
      currentConfigPin = p;
      document.getElementById('modalTitle').innerText = 'Configurar GPIO ' + p;
      var currentMode = pinConfigs[p].mode;
      if (currentMode === 'output') {
        document.getElementById('radioOutput').checked = true;
      } else {
        document.getElementById('radioInput').checked = true;
      }
      document.getElementById('modalConfig').style.display = 'flex';
    }

    function closeModal() {
      document.getElementById('modalConfig').style.display = 'none';
      currentConfigPin = null;
    }

    function savePinMode() {
      if (!currentConfigPin) return;
      var selected = document.querySelector('input[name="pinModeChoice"]:checked').value;
      var modeInt = selected === 'output' ? 0 : 1;
      var pinToSave = currentConfigPin;

      fetch('/api/mode?pin=' + pinToSave + '&mode=' + modeInt)
        .then(function(r) { return r.json(); })
        .then(function(data) {
          pinConfigs[pinToSave].mode = selected;
          closeModal();
          renderExtraGPIOs();
          updateStatus();
        })
        .catch(function(e) {
          console.error(e);
          closeModal();
        });
    }

    function togglePin(p) {
      fetch('/api/toggle?pin=' + p)
        .then(function(r) { return r.json(); })
        .then(function(data) {
          updateStatus();
        })
        .catch(function(e) {
          console.error('Erro no toggle:', e);
        });
    }

    function pulsePin(p, ms) {
      fetch('/api/pulse?pin=' + p + '&ms=' + ms)
        .then(function(r) { return r.json(); })
        .then(function(data) {
          updateStatus();
        })
        .catch(function(e) {
          console.error('Erro no pulse:', e);
        });
    }

    function setRelays(val) {
      fetch('/api/relays?state=' + val)
        .then(function(r) { return r.json(); })
        .then(function(data) {
          updateStatus();
        })
        .catch(function(e) {
          console.error('Erro no setRelays:', e);
        });
    }

    function setAll(val) {
      fetch('/api/all?state=' + val)
        .then(function(r) { return r.json(); })
        .then(function(data) {
          updateStatus();
        })
        .catch(function(e) {
          console.error('Erro no setAll:', e);
        });
    }

    function updateStatus() {
      fetch('/api/status')
        .then(function(r) { return r.json(); })
        .then(function(data) {
          if (data.uptime !== undefined) {
            var el = document.getElementById('uptimeBadge');
            if (el) el.innerText = 'Uptime: ' + data.uptime + 's';
          }

          if (data.pins) {
            data.pins.forEach(function(p) {
              // Verifica se é um dos 8 relés
              if (relayStates[p.pin] !== undefined) {
                relayStates[p.pin] = p.val;
                var card = document.getElementById('relay_card_' + p.pin);
                var pill = document.getElementById('relay_pill_' + p.pin);
                var btn = document.getElementById('relay_btn_' + p.pin);
                if (card && pill && btn) {
                  if (p.val === 1) {
                    card.className = 'relay-card active';
                    pill.className = 'status-pill status-on';
                    pill.innerText = 'LIGADO (FECHADO)';
                    btn.className = 'btn btn-red';
                    btn.innerText = 'Desligar';
                  } else {
                    card.className = 'relay-card';
                    pill.className = 'status-pill status-off';
                    pill.innerText = 'DESLIGADO (ABERTO)';
                    btn.className = 'btn btn-green';
                    btn.innerText = 'Ligar Relé';
                  }
                }
              }

              // LED Onboard (GPIO 5)
              if (p.pin === 5) {
                var cardLed = document.getElementById('cardLed');
                var pillLed = document.getElementById('pillLed');
                if (cardLed && pillLed) {
                  if (p.val === 1) {
                    cardLed.className = 'hero-card led-active';
                    pillLed.className = 'status-pill status-led-on';
                    pillLed.innerText = 'LIGADO (HIGH)';
                  } else {
                    cardLed.className = 'hero-card';
                    pillLed.className = 'status-pill status-off';
                    pillLed.innerText = 'DESLIGADO (LOW)';
                  }
                }
              }

              // Botão Boot / IO0 (GPIO 0)
              if (p.pin === 0) {
                var cardBoot = document.getElementById('cardBoot');
                var pillBoot = document.getElementById('pillBoot');
                if (cardBoot && pillBoot) {
                  if (p.val === 0) {
                    cardBoot.className = 'hero-card active';
                    pillBoot.className = 'status-pill status-pressed';
                    pillBoot.innerText = 'PRESSIONADO (LOW)';
                  } else {
                    cardBoot.className = 'hero-card';
                    pillBoot.className = 'status-pill status-off';
                    pillBoot.innerText = 'SOLTO (HIGH)';
                  }
                }
              }

              // GPIOs da grade de expansão
              if (pinConfigs[p.pin]) {
                var modeStr = p.mode === 0 ? 'output' : 'input';
                var changedMode = pinConfigs[p.pin].mode !== modeStr;
                pinConfigs[p.pin].mode = modeStr;
                pinConfigs[p.pin].val = p.val;

                if (changedMode) {
                  renderExtraGPIOs();
                } else {
                  var gCard = document.getElementById('card_g_' + p.pin);
                  if (gCard) {
                    if (modeStr === 'output') {
                      gCard.className = 'gpio-card' + (p.val === 1 ? ' output-on' : '');
                      var b = document.getElementById('btoggle_' + p.pin);
                      if (b) {
                        b.className = 'btn-toggle state-' + p.val;
                        b.innerText = p.val === 1 ? 'LIGADO (HIGH)' : 'DESLIGADO (LOW)';
                      }
                    } else {
                      gCard.className = 'gpio-card input-mode';
                      var inb = document.getElementById('inbadge_' + p.pin);
                      if (inb) {
                        inb.className = 'input-badge input-' + p.val;
                        inb.innerText = 'LEITURA: ' + (p.val === 1 ? 'HIGH (1)' : 'LOW (0)');
                      }
                    }
                  }
                }
              }
            });
          }
        })
        .catch(function(e) {
          console.error('Erro no updateStatus:', e);
        });
    }

    renderRelays();
    renderExtraGPIOs();
    updateStatus();
    setInterval(updateStatus, 800);
  </script>
</body>
</html>
)rawliteral";
