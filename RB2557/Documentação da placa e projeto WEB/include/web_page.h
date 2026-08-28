#pragma once

// ====================================================================
// ROBOBUILDERS RB2557 - PAINEL "HELLO WORLD" DE TESTE DE PLACA
// Modo Escuro (Dark Mode) • Teste de Hardware Rápido • Mapeamento Input/Output
// ====================================================================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>ROBOBUILDERS RB2557 - Teste de Placa</title>
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
    body { background: var(--bg); color: var(--text-main); padding: 14px; max-width: 920px; margin: 0 auto; line-height: 1.4; }

    /* Top Header */
    header { background: var(--card-bg); border: 1px solid var(--card-border); border-radius: var(--radius); padding: 18px 16px; margin-bottom: 18px; text-align: center; box-shadow: var(--shadow-sm); }
    h1 { font-size: 1.45rem; color: var(--primary); margin-bottom: 4px; font-weight: 700; }
    p.sub { font-size: 0.88rem; color: var(--text-muted); }
    .badge-bar { display: flex; flex-wrap: wrap; justify-content: center; gap: 8px; margin-top: 12px; }
    .badge { background: #1e293b; border: 1px solid #334155; padding: 5px 12px; border-radius: 20px; font-size: 0.78rem; color: #cbd5e1; font-weight: 500; }
    .badge.live { border-color: #10b981; background: rgba(16, 185, 129, 0.15); color: #34d399; font-weight: 600; }

    /* Section Titles */
    .section-title { font-size: 1.1rem; color: #f1f5f9; margin: 20px 0 12px 4px; font-weight: 700; display: flex; align-items: center; justify-content: space-between; }

    /* Hero Cards (LED, Relay, Boot) */
    .hero-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(260px, 1fr)); gap: 14px; margin-bottom: 18px; }
    .hero-card { background: var(--card-bg); border: 2px solid var(--card-border); border-radius: var(--radius); padding: 18px 16px; text-align: center; box-shadow: var(--shadow-sm); transition: all 0.2s; }
    .hero-card.active { border-color: var(--green); background: rgba(16, 185, 129, 0.08); }
    .hero-card.led-active { border-color: var(--primary); background: rgba(14, 165, 233, 0.08); }
    .hero-card h3 { font-size: 1.12rem; margin-bottom: 4px; color: #f8fafc; }
    .hero-card .pin-tag { font-size: 0.82rem; color: var(--text-muted); margin-bottom: 12px; display: block; }
    .status-pill { display: inline-block; padding: 5px 16px; border-radius: 20px; font-weight: 700; font-size: 0.85rem; margin-bottom: 14px; letter-spacing: 0.3px; }
    .status-off { background: #1e293b; color: #94a3b8; border: 1px solid #334155; }
    .status-on { background: var(--green); color: #ffffff; }
    .status-led-on { background: var(--primary); color: #ffffff; }
    .status-pressed { background: #854d0e; color: #fef08a; border: 1px solid #facc15; }

    /* Action Buttons */
    .btn-group { display: flex; gap: 8px; justify-content: center; }
    .btn { cursor: pointer; border: none; padding: 10px 16px; border-radius: 8px; font-weight: 600; font-size: 0.88rem; transition: background 0.15s, transform 0.1s; display: inline-flex; align-items: center; justify-content: center; gap: 6px; min-height: 42px; }
    .btn:active { transform: scale(0.97); }
    .btn-primary { background: var(--primary); color: #fff; }
    .btn-primary:hover { background: var(--primary-hover); }
    .btn-green { background: var(--green); color: #fff; }
    .btn-green:hover { background: var(--green-hover); }
    .btn-red { background: var(--red); color: #fff; }
    .btn-red:hover { background: var(--red-hover); }
    .btn-secondary { background: #1e293b; color: #cbd5e1; border: 1px solid #334155; }
    .btn-secondary:hover { background: #334155; }

    /* Bulk Bar */
    .bulk-bar { display: flex; gap: 8px; margin-bottom: 12px; flex-wrap: wrap; }

    /* GPIO Grid */
    .gpio-grid { display: grid; grid-template-columns: repeat(auto-fill, minmax(160px, 1fr)); gap: 10px; }
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

    /* Modal de Configuração de GPIO */
    .modal-overlay { position: fixed; inset: 0; background: rgba(0, 0, 0, 0.75); display: none; align-items: center; justify-content: center; z-index: 999; padding: 14px; }
    .modal { background: #131b2e; border: 1px solid #334155; border-radius: var(--radius); padding: 20px; max-width: 360px; width: 100%; box-shadow: var(--shadow-md); color: var(--text-main); }
    .modal h3 { font-size: 1.15rem; color: #f8fafc; margin-bottom: 12px; }
    .modal p { font-size: 0.85rem; color: var(--text-muted); margin-bottom: 16px; }
    .radio-option { display: flex; align-items: center; gap: 10px; padding: 10px; border: 1px solid #1e293b; border-radius: 8px; margin-bottom: 10px; cursor: pointer; font-weight: 500; font-size: 0.9rem; background: #0b0f19; }
    .radio-option:hover { background: #1a233a; }
    .modal-actions { display: flex; justify-content: flex-end; gap: 8px; margin-top: 18px; }

    /* Footer */
    footer { text-align: center; font-size: 0.8rem; color: var(--text-muted); margin-top: 24px; padding: 14px; border-top: 1px solid var(--card-border); }
  </style>
</head>
<body>

  <!-- CABEÇALHO ESCURO SEM ÍCONE DE ROBO -->
  <header>
    <h1>ROBOBUILDERS RB2557</h1>
    <p class="sub">Painel de Teste de Hardware & Mapeamento de GPIOs</p>
    <div class="badge-bar">
      <span class="badge live">● Conectado (192.168.4.1)</span>
      <span class="badge">Wi-Fi: ROBOBUILDERS-RB2557</span>
      <span class="badge" id="uptimeBadge">Uptime: 0s</span>
    </div>
  </header>

  <!-- 1. TESTE DOS PRINCIPAIS RECURSOS -->
  <div class="section-title">1. Teste Rápido de Hardware</div>
  <div class="hero-grid">
    
    <!-- LED ONBOARD (GPIO 23) -->
    <div class="hero-card" id="cardLed">
      <h3>LED Onboard</h3>
      <span class="pin-tag">Pino Físico: <b>GPIO 23</b></span>
      <div id="pillLed" class="status-pill status-off">DESLIGADO</div>
      <div class="btn-group">
        <button class="btn btn-primary" onclick="togglePin(23)">Ligar / Desligar</button>
        <button class="btn btn-secondary" onclick="pulsePin(23, 500)">Piscar</button>
      </div>
    </div>

    <!-- RELÉ PRINCIPAL (GPIO 16) -->
    <div class="hero-card" id="cardRelay">
      <h3>Relé de Potência</h3>
      <span class="pin-tag">Pino Físico: <b>GPIO 16</b></span>
      <div id="pillRelay" class="status-pill status-off">DESLIGADO (ABERTO)</div>
      <div class="btn-group">
        <button class="btn btn-green" id="btnRelay" onclick="togglePin(16)">Acionar Relé</button>
        <button class="btn btn-secondary" onclick="pulsePin(16, 1000)">Pulso 1s</button>
      </div>
    </div>

    <!-- BOTÃO BOOT (GPIO 0) -->
    <div class="hero-card" id="cardBoot">
      <h3>Botão Boot Físico</h3>
      <span class="pin-tag">Pino Físico: <b>GPIO 0</b></span>
      <div id="pillBoot" class="status-pill status-off">SOLTO (HIGH)</div>
      <p style="font-size:0.78rem; color:var(--text-muted); margin-top:8px;">Pressione o botão BOOT na placa para ver a leitura em tempo real.</p>
    </div>

  </div>

  <!-- 2. TODAS AS GPIOS COM ENGRENAGEM DE CONFIGURAÇÃO -->
  <div class="section-title">
    <span>2. Mapeamento & Controle de GPIOs</span>
  </div>
  
  <div class="bulk-bar">
    <button class="btn btn-secondary" onclick="setAll(1)">Ligar Todas as Saídas</button>
    <button class="btn btn-secondary" onclick="setAll(0)">Desligar Todas</button>
  </div>
  
  <div class="gpio-grid" id="gpioContainer">
    <!-- Renderizado dinamicamente via JS -->
  </div>

  <!-- MODAL DE CONFIGURAÇÃO DE MODO (INPUT / OUTPUT) -->
  <div class="modal-overlay" id="modalConfig">
    <div class="modal">
      <h3 id="modalTitle">Configurar GPIO</h3>
      <p>Selecione a função deste pino:</p>
      
      <label class="radio-option">
        <input type="radio" name="pinModeChoice" value="output" id="radioOutput">
        <div>
          <b>OUTPUT (Saída Digital)</b>
          <div style="font-size:0.75rem; color:#94a3b8;">Permite ligar e desligar cargas ou LEDs</div>
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
    ROBOBUILDERS RB2557 • Teste de Hardware de Fábrica
  </footer>

  <script>
    // Pinos de GPIO mapeados
    const gpioList = [2, 4, 5, 12, 13, 14, 15, 17, 18, 19, 21, 22, 25, 26, 27, 32, 33, 34, 35];
    let pinConfigs = {}; // { pin: { mode: 'output'|'input', val: 0 } }
    let currentConfigPin = null;

    // Inicializa estrutura
    gpioList.forEach(p => {
      // Pinos 34 e 35 são apenas entrada no hardware do ESP32
      const isInputOnly = (p === 34 || p === 35);
      pinConfigs[p] = { mode: isInputOnly ? 'input' : 'output', val: 0 };
    });

    function renderGPIOs() {
      const container = document.getElementById('gpioContainer');
      container.innerHTML = '';

      gpioList.forEach(p => {
        const cfg = pinConfigs[p];
        const isInputOnly = (p === 34 || p === 35);
        const card = document.createElement('div');
        card.className = 'gpio-card' + (cfg.mode === 'output' && cfg.val === 1 ? ' output-on' : '') + (cfg.mode === 'input' ? ' input-mode' : '');
        card.id = 'card_g_' + p;

        let actionHtml = '';
        if (cfg.mode === 'output') {
          actionHtml = `
            <button class="btn-toggle state-${cfg.val}" id="btoggle_${p}" onclick="togglePin(${p})">
              ${cfg.val === 1 ? 'LIGADO (HIGH)' : 'DESLIGADO (LOW)'}
            </button>
          `;
        } else {
          actionHtml = `
            <span class="input-badge input-${cfg.val}" id="inbadge_${p}">
              LEITURA: ${cfg.val === 1 ? 'HIGH (1)' : 'LOW (0)'}
            </span>
          `;
        }

        const gearHtml = isInputOnly ? '' : `
          <button class="gear-btn" title="Configurar Input/Output" onclick="openModal(${p})">⚙️</button>
        `;

        card.innerHTML = `
          <div>
            <div class="gpio-head">
              <span class="gpio-name">GPIO ${p}</span>
              ${gearHtml}
            </div>
            <span class="gpio-mode-tag ${cfg.mode === 'output' ? 'mode-output' : 'mode-input'}" id="modetag_${p}">
              ${cfg.mode.toUpperCase()}
            </span>
          </div>
          <div class="gpio-action">
            ${actionHtml}
          </div>
        `;
        container.appendChild(card);
      });
    }

    // Modal de Configuração
    function openModal(p) {
      currentConfigPin = p;
      document.getElementById('modalTitle').innerText = `Configurar GPIO ${p}`;
      const currentMode = pinConfigs[p].mode;
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
      const selected = document.querySelector('input[name="pinModeChoice"]:checked').value;
      const modeInt = selected === 'output' ? 0 : 1;

      fetch(`/api/mode?pin=${currentConfigPin}&mode=${modeInt}`)
        .then(r => r.json())
        .then(data => {
          pinConfigs[currentConfigPin].mode = selected;
          closeModal();
          renderGPIOs();
          updateStatus();
        })
        .catch(e => {
          console.error(e);
          closeModal();
        });
    }

    // Controle de GPIO
    function togglePin(p) {
      fetch('/api/toggle?pin=' + p)
        .then(r => r.json())
        .then(data => { updateStatus(); })
        .catch(e => console.error(e));
    }

    function pulsePin(p, ms) {
      fetch(`/api/pulse?pin=${p}&ms=${ms}`)
        .then(r => r.json())
        .then(data => { updateStatus(); })
        .catch(e => console.error(e));
    }

    function setAll(val) {
      fetch('/api/all?state=' + val)
        .then(r => r.json())
        .then(data => { updateStatus(); })
        .catch(e => console.error(e));
    }

    // Polling de Status
    function updateStatus() {
      fetch('/api/status')
        .then(r => r.json())
        .then(data => {
          if (data.uptime !== undefined) {
            document.getElementById('uptimeBadge').innerText = `Uptime: ${data.uptime}s`;
          }

          if (data.pins) {
            data.pins.forEach(p => {
              // LED Onboard (GPIO 23)
              if (p.pin === 23) {
                const card = document.getElementById('cardLed');
                const pill = document.getElementById('pillLed');
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

              // Relé (GPIO 16)
              if (p.pin === 16) {
                const card = document.getElementById('cardRelay');
                const pill = document.getElementById('pillRelay');
                const btn = document.getElementById('btnRelay');
                if (p.val === 1) {
                  card.className = 'hero-card active';
                  pill.className = 'status-pill status-on';
                  pill.innerText = 'LIGADO (FECHADO)';
                  btn.className = 'btn btn-red';
                  btn.innerText = 'Desligar Relé';
                } else {
                  card.className = 'hero-card';
                  pill.className = 'status-pill status-off';
                  pill.innerText = 'DESLIGADO (ABERTO)';
                  btn.className = 'btn btn-green';
                  btn.innerText = 'Acionar Relé';
                }
              }

              // Botão Boot (GPIO 0)
              if (p.pin === 0) {
                const card = document.getElementById('cardBoot');
                const pill = document.getElementById('pillBoot');
                if (p.val === 0) {
                  card.className = 'hero-card active';
                  pill.className = 'status-pill status-pressed';
                  pill.innerText = 'PRESSIONADO (LOW)';
                } else {
                  card.className = 'hero-card';
                  pill.className = 'status-pill status-off';
                  pill.innerText = 'SOLTO (HIGH)';
                }
              }

              // GPIOs da grade
              if (pinConfigs[p.pin]) {
                const modeStr = p.mode === 0 ? 'output' : 'input';
                const changedMode = pinConfigs[p.pin].mode !== modeStr;
                pinConfigs[p.pin].mode = modeStr;
                pinConfigs[p.pin].val = p.val;

                if (changedMode) {
                  renderGPIOs();
                } else {
                  const card = document.getElementById('card_g_' + p.pin);
                  if (card) {
                    if (modeStr === 'output') {
                      card.className = 'gpio-card' + (p.val === 1 ? ' output-on' : '');
                      const b = document.getElementById('btoggle_' + p.pin);
                      if (b) {
                        b.className = 'btn-toggle state-' + p.val;
                        b.innerText = p.val === 1 ? 'LIGADO (HIGH)' : 'DESLIGADO (LOW)';
                      }
                    } else {
                      card.className = 'gpio-card input-mode';
                      const inb = document.getElementById('inbadge_' + p.pin);
                      if (inb) {
                        inb.className = 'input-badge input-' + p.val;
                        inb.innerText = `LEITURA: ${p.val === 1 ? 'HIGH (1)' : 'LOW (0)'}`;
                      }
                    }
                  }
                }
              }
            });
          }
        })
        .catch(e => console.error(e));
    }

    renderGPIOs();
    updateStatus();
    setInterval(updateStatus, 800);
  </script>
</body>
</html>
)rawliteral";
