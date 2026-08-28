#pragma once

// ====================================================================
// ROBOBUILDERS RB2555 (ESP-32_RELAY_30A_X2_V1.1) - PAINEL DE TESTE
// LED Onboard: GPIO 5 | Relé 1: GPIO 13 | Relé 2: GPIO 12 | Botão Boot: GPIO 0 (IO0)
// ====================================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>ROBOBUILDERS RB2555 - Teste de Placa (ESP-32 30A X2)</title>
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
      --radius: 12px;
      --shadow-sm: 0 2px 4px rgba(0,0,0,0.3);
      --shadow-md: 0 8px 16px rgba(0,0,0,0.4);
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
    body { background: var(--bg); color: var(--text-main); padding: 14px; max-width: 960px; margin: 0 auto; line-height: 1.4; }

    header { background: var(--card-bg); border: 1px solid var(--card-border); border-radius: var(--radius); padding: 18px 16px; margin-bottom: 18px; text-align: center; box-shadow: var(--shadow-sm); }
    h1 { font-size: 1.45rem; color: var(--primary); margin-bottom: 4px; font-weight: 700; }
    p.sub { font-size: 0.88rem; color: var(--text-muted); }
    .badge-bar { display: flex; flex-wrap: wrap; justify-content: center; gap: 8px; margin-top: 12px; }
    .badge { background: #1e293b; border: 1px solid #334155; padding: 5px 12px; border-radius: 20px; font-size: 0.78rem; color: #cbd5e1; font-weight: 500; }
    .badge.live { border-color: #10b981; background: rgba(16, 185, 129, 0.15); color: #34d399; font-weight: 600; }

    .section-title { font-size: 1.1rem; color: #f1f5f9; margin: 20px 0 12px 4px; font-weight: 700; display: flex; align-items: center; justify-content: space-between; }

    .hero-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 14px; margin-bottom: 18px; }
    .hero-card { background: var(--card-bg); border: 2px solid var(--card-border); border-radius: var(--radius); padding: 18px 16px; text-align: center; box-shadow: var(--shadow-sm); transition: all 0.2s; }
    .hero-card.active { border-color: var(--green); background: rgba(16, 185, 129, 0.08); }
    .hero-card.led-active { border-color: var(--primary); background: rgba(14, 165, 233, 0.08); }
    .hero-card h3 { font-size: 1.08rem; margin-bottom: 4px; color: #f8fafc; }
    .hero-card .pin-tag { font-size: 0.82rem; color: var(--text-muted); margin-bottom: 12px; display: block; }
    .status-pill { display: inline-block; padding: 5px 14px; border-radius: 20px; font-weight: 700; font-size: 0.82rem; margin-bottom: 14px; letter-spacing: 0.3px; }
    .status-off { background: #1e293b; color: #94a3b8; border: 1px solid #334155; }
    .status-on { background: var(--green); color: #ffffff; }
    .status-led-on { background: var(--primary); color: #ffffff; }
    .status-pressed { background: #854d0e; color: #fef08a; border: 1px solid #facc15; }

    .btn-group { display: flex; gap: 8px; justify-content: center; }
    .btn { cursor: pointer; border: none; padding: 10px 14px; border-radius: 8px; font-weight: 600; font-size: 0.86rem; transition: background 0.15s, transform 0.1s; display: inline-flex; align-items: center; justify-content: center; gap: 6px; min-height: 40px; user-select: none; }
    .btn:active { transform: scale(0.97); }
    .btn-primary { background: var(--primary); color: #fff; }
    .btn-primary:hover { background: var(--primary-hover); }
    .btn-green { background: var(--green); color: #fff; }
    .btn-green:hover { background: var(--green-hover); }
    .btn-red { background: var(--red); color: #fff; }
    .btn-red:hover { background: var(--red-hover); }
    .btn-secondary { background: #1e293b; color: #cbd5e1; border: 1px solid #334155; }
    .btn-secondary:hover { background: #334155; }

    .bulk-bar { display: flex; gap: 8px; margin-bottom: 12px; flex-wrap: wrap; }

    .gpio-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(150px, 1fr)); gap: 10px; }
    .gpio-card { background: var(--card-bg); border: 1px solid var(--card-border); border-radius: var(--radius); padding: 12px; box-shadow: var(--shadow-sm); transition: all 0.15s; display: flex; flex-direction: column; justify-content: space-between; }
    .gpio-card.output-on { border-color: var(--green); background: rgba(16, 185, 129, 0.08); }
    .gpio-card.input-mode { border-style: dashed; background: #0e1626; }
    
    .gpio-head { display: flex; justify-content: space-between; align-items: center; margin-bottom: 8px; }
    .gpio-name { font-weight: 700; font-size: 0.92rem; color: #f1f5f9; }
    .gear-btn { background: #1e293b; border: 1px solid #334155; border-radius: 6px; width: 28px; height: 28px; display: inline-flex; align-items: center; justify-content: center; cursor: pointer; color: #94a3b8; font-size: 0.9rem; transition: background 0.15s; }
    .gear-btn:hover { background: #334155; color: #f8fafc; }

    .gpio-mode-tag { font-size: 0.72rem; padding: 2px 6px; border-radius: 4px; font-weight: 600; display: inline-block; margin-bottom: 8px; width: fit-content; }
    .mode-output { background: rgba(14, 165, 233, 0.2); color: #38bdf8; }
    .mode-input { background: rgba(245, 158, 11, 0.2); color: #fbbf24; }

    .gpio-action { margin-top: 4px; }
    .btn-toggle { width: 100%; padding: 8px 4px; border-radius: 6px; border: 1px solid #334155; font-weight: 700; font-size: 0.82rem; cursor: pointer; transition: all 0.15s; }
    .btn-toggle.state-0 { background: #1e293b; color: #94a3b8; }
    .btn-toggle.state-1 { background: var(--green); color: #ffffff; border-color: var(--green); }
    
    .input-badge { padding: 8px 4px; border-radius: 6px; font-weight: 700; font-size: 0.82rem; text-align: center; display: block; border: 1px solid #334155; }
    .input-0 { background: #1e293b; color: #94a3b8; }
    .input-1 { background: rgba(14, 165, 233, 0.25); color: #38bdf8; border-color: rgba(14, 165, 233, 0.5); }

    .modal-overlay { position: fixed; inset: 0; background: rgba(0, 0, 0, 0.75); display: none; align-items: center; justify-content: center; z-index: 999; padding: 14px; }
    .modal { background: #131b2e; border: 1px solid #334155; border-radius: var(--radius); padding: 20px; max-width: 360px; width: 100%; box-shadow: var(--shadow-md); color: var(--text-main); }
    .modal h3 { font-size: 1.15rem; color: #f8fafc; margin-bottom: 12px; }
    .modal p { font-size: 0.85rem; color: var(--text-muted); margin-bottom: 16px; }
    .radio-option { display: flex; align-items: center; gap: 10px; padding: 10px; border: 1px solid #1e293b; border-radius: 8px; margin-bottom: 10px; cursor: pointer; font-weight: 500; font-size: 0.9rem; background: #0b0f19; }
    .radio-option:hover { background: #1a233a; }
    .modal-actions { display: flex; justify-content: flex-end; gap: 8px; margin-top: 18px; }

    footer { text-align: center; font-size: 0.8rem; color: var(--text-muted); margin-top: 24px; padding: 14px; border-top: 1px solid var(--card-border); }
  </style>
</head>
<body>

  <header>
    <h1>ROBOBUILDERS RB2555</h1>
    <p class="sub">ESP-32_RELAY_30A_X2_V1.1 &bull; Painel de Teste e Diagn&oacute;stico de Hardware</p>
    <div class="badge-bar">
      <span class="badge live">&bull; Conectado (192.168.4.1)</span>
      <span class="badge">Wi-Fi: ROBOBUILDERS-RB2555</span>
      <span class="badge" id="uptimeBadge">Uptime: 0s</span>
    </div>
  </header>

  <div class="section-title">1. Teste R&aacute;pido dos Recursos Onboard (ESP32 Relay 30A X2)</div>
  <div class="hero-grid">
    
    <!-- LED ONBOARD (GPIO 5) -->
    <div class="hero-card" id="cardLed">
      <h3>LED Onboard</h3>
      <span class="pin-tag">Pino F&iacute;sico: <b>GPIO 5</b></span>
      <div id="pillLed" class="status-pill status-off">DESLIGADO</div>
      <div class="btn-group">
        <button class="btn btn-primary" onclick="togglePin(5)">Ligar / Desligar</button>
        <button class="btn btn-secondary" onclick="pulsePin(5, 500)">Piscar</button>
      </div>
    </div>

    <!-- REL&Eacute; 1 DE POT&Ecirc;NCIA 30A (GPIO 13) -->
    <div class="hero-card" id="cardRelay1">
      <h3>Rel&eacute; 1 (30A - K1)</h3>
      <span class="pin-tag">Pino F&iacute;sico: <b>GPIO 13</b></span>
      <div id="pillRelay1" class="status-pill status-off">DESLIGADO (ABERTO)</div>
      <div class="btn-group">
        <button class="btn btn-green" id="btnRelay1" onclick="togglePin(13)">Acionar Rel&eacute; 1</button>
        <button class="btn btn-secondary" onclick="pulsePin(13, 1000)">Pulso 1s</button>
      </div>
    </div>

    <!-- REL&Eacute; 2 DE POT&Ecirc;NCIA 30A (GPIO 12) -->
    <div class="hero-card" id="cardRelay2">
      <h3>Rel&eacute; 2 (30A - K2)</h3>
      <span class="pin-tag">Pino F&iacute;sico: <b>GPIO 12</b></span>
      <div id="pillRelay2" class="status-pill status-off">DESLIGADO (ABERTO)</div>
      <div class="btn-group">
        <button class="btn btn-green" id="btnRelay2" onclick="togglePin(12)">Acionar Rel&eacute; 2</button>
        <button class="btn btn-secondary" onclick="pulsePin(12, 1000)">Pulso 1s</button>
      </div>
    </div>

    <!-- BOT&Atilde;O BOOT / IO0 (GPIO 0) -->
    <div class="hero-card" id="cardBoot">
      <h3>Bot&atilde;o Boot / IO0</h3>
      <span class="pin-tag">Pino F&iacute;sico: <b>GPIO 0</b></span>
      <div id="pillBoot" class="status-pill status-off">SOLTO (HIGH)</div>
      <p style="font-size:0.78rem; color:var(--text-muted); margin-top:8px;">Pressione o bot&atilde;o IO0/BOOT na placa para ver a leitura mudar em tempo real.</p>
    </div>

  </div>

  <div class="section-title">
    <span>2. Mapeamento &amp; Controle de Todas as GPIOs</span>
  </div>
  
  <div class="bulk-bar">
    <button class="btn btn-secondary" onclick="setAll(1)">Ligar Todas as Sa&iacute;das</button>
    <button class="btn btn-secondary" onclick="setAll(0)">Desligar Todas</button>
  </div>
  
  <div class="gpio-grid" id="gpioContainer"></div>

  <!-- MODAL DE CONFIGURA&Ccedil;&Atilde;O DE MODO -->
  <div class="modal-overlay" id="modalConfig">
    <div class="modal">
      <h3 id="modalTitle">Configurar GPIO</h3>
      <p>Selecione a fun&ccedil;&atilde;o deste pino:</p>
      
      <label class="radio-option">
        <input type="radio" name="pinModeChoice" value="output" id="radioOutput">
        <div>
          <b>OUTPUT (Sa&iacute;da Digital)</b>
          <div style="font-size:0.75rem; color:#94a3b8;">Permite ligar e desligar cargas ou perif&eacute;ricos</div>
        </div>
      </label>

      <label class="radio-option">
        <input type="radio" name="pinModeChoice" value="input" id="radioInput">
        <div>
          <b>INPUT (Entrada Digital com Pull-Up)</b>
          <div style="font-size:0.75rem; color:#94a3b8;">L&ecirc; sinais de bot&otilde;es e sensores (HIGH/LOW)</div>
        </div>
      </label>

      <div class="modal-actions">
        <button class="btn btn-secondary" onclick="closeModal()">Cancelar</button>
        <button class="btn btn-primary" onclick="savePinMode()">Salvar</button>
      </div>
    </div>
  </div>

  <footer>
    ROBOBUILDERS RB2555 (ESP-32_RELAY_30A_X2_V1.1) &bull; Teste de Hardware de F&aacute;brica
  </footer>

  <script>
    // Lista de GPIOs de expansão (GPIO 5 = LED, GPIO 13 = Relé 1, GPIO 12 = Relé 2, GPIO 0 = Botão Boot)
    const gpioList = [2, 4, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35];
    let pinConfigs = {};
    let currentConfigPin = null;

    gpioList.forEach(function(p) {
      var isInputOnly = (p === 34 || p === 35);
      pinConfigs[p] = { mode: isInputOnly ? 'input' : 'output', val: 0 };
    });

    function renderGPIOs() {
      var container = document.getElementById('gpioContainer');
      if (!container) return;
      container.innerHTML = '';

      gpioList.forEach(function(p) {
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

        var gearHtml = isInputOnly ? '' : '<button class="gear-btn" title="Configurar Input/Output" onclick="openModal(' + p + ')">&#9881;&#65039;</button>';

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
          '<div class="gpio-action">' +
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
          renderGPIOs();
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
              // LED Onboard (GPIO 5)
              if (p.pin === 5) {
                var card = document.getElementById('cardLed');
                var pill = document.getElementById('pillLed');
                if (card && pill) {
                  if (p.val === 1) {
                    card.className = 'hero-card led-active';
                    pill.className = 'status-pill status-led-on';
                    pill.innerText = 'LIGADO (HIGH)';
                  } else {
                    card.className = 'hero-card';
                    pill.className = 'status-pill status-off';
                    pill.innerText = 'DESLIGADO (LOW)';
                  }
                }
              }

              // Relé 1 (30A - GPIO 13)
              if (p.pin === 13) {
                var cardRelay1 = document.getElementById('cardRelay1');
                var pillRelay1 = document.getElementById('pillRelay1');
                var btnRelay1 = document.getElementById('btnRelay1');
                if (cardRelay1 && pillRelay1 && btnRelay1) {
                  if (p.val === 1) {
                    cardRelay1.className = 'hero-card active';
                    pillRelay1.className = 'status-pill status-on';
                    pillRelay1.innerText = 'LIGADO (FECHADO)';
                    btnRelay1.className = 'btn btn-red';
                    btnRelay1.innerText = 'Desligar Relé 1';
                  } else {
                    cardRelay1.className = 'hero-card';
                    pillRelay1.className = 'status-pill status-off';
                    pillRelay1.innerText = 'DESLIGADO (ABERTO)';
                    btnRelay1.className = 'btn btn-green';
                    btnRelay1.innerText = 'Acionar Relé 1';
                  }
                }
              }

              // Relé 2 (30A - GPIO 12)
              if (p.pin === 12) {
                var cardRelay2 = document.getElementById('cardRelay2');
                var pillRelay2 = document.getElementById('pillRelay2');
                var btnRelay2 = document.getElementById('btnRelay2');
                if (cardRelay2 && pillRelay2 && btnRelay2) {
                  if (p.val === 1) {
                    cardRelay2.className = 'hero-card active';
                    pillRelay2.className = 'status-pill status-on';
                    pillRelay2.innerText = 'LIGADO (FECHADO)';
                    btnRelay2.className = 'btn btn-red';
                    btnRelay2.innerText = 'Desligar Relé 2';
                  } else {
                    cardRelay2.className = 'hero-card';
                    pillRelay2.className = 'status-pill status-off';
                    pillRelay2.innerText = 'DESLIGADO (ABERTO)';
                    btnRelay2.className = 'btn btn-green';
                    btnRelay2.innerText = 'Acionar Relé 2';
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

              // GPIOs da grade
              if (pinConfigs[p.pin]) {
                var modeStr = p.mode === 0 ? 'output' : 'input';
                var changedMode = pinConfigs[p.pin].mode !== modeStr;
                pinConfigs[p.pin].mode = modeStr;
                pinConfigs[p.pin].val = p.val;

                if (changedMode) {
                  renderGPIOs();
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

    renderGPIOs();
    updateStatus();
    setInterval(updateStatus, 800);
  </script>
</body>
</html>
)rawliteral";
