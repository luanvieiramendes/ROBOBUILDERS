# 📘 DOCUMENTAÇÃO TÉCNICA: ROBOBUILDERS RB2555
### *Firmware de Teste e Diagnóstico de Hardware • ESP32 Relay 30A X2 V1.1 (`ESP-32_RELAY_30A_X2_V1.1`)*

---

## 📋 Sumário
- [1. Visão Geral do Projeto](#1-visão-geral-do-projeto)
- [2. Especificações de Hardware](#2-especificações-de-hardware)
- [3. Mapeamento de Pinos (Pinout Oficial)](#3-mapeamento-de-pinos-pinout-oficial)
- [4. Recursos do Painel Web](#4-recursos-do-painel-web)
- [5. Documentação da API REST](#5-documentação-da-api-rest)
- [6. Guia de Conexão e Gravação UART (Passo a Passo)](#6-guia-de-conexão-e-gravação-uart-passo-a-passo)
- [7. Estrutura do Repositório](#7-estrutura-do-repositório)
- [8. Como Compilar e Enviar](#8-como-compilar-e-enviar)
- [9. Acesso ao Painel Web](#9-acesso-ao-painel-web)

---

## 1. Visão Geral do Projeto

O **ROBOBUILDERS RB2555** é o firmware de teste e validação de hardware (*Board Diagnostic / Factory Hardware Validation*) desenvolvido especialmente para a placa **`ESP-32_RELAY_30A_X2_V1.1`** (equipada com módulo **ESP32-WROOM-32 / 32E** e **2 Relés de Potência de 30A**).

O sistema cria um **Ponto de Acesso Wi-Fi autônomo** com **Portal Captivo** integrado e uma interface web moderna em **Modo Escuro (Dark Mode)**, permitindo o acionamento em tempo real dos relés de 30A, LED de status onboard, leitura dinâmica do botão físico e controle de todos os barramentos de GPIOs diretamente pelo smartphone ou computador, sem necessidade de conexão com a internet.

---

## 2. Especificações de Hardware

| Item | Especificação |
| :--- | :--- |
| **Identificador do Produto** | ROBOBUILDERS RB2555 |
| **Modelo da Placa Base** | `ESP-32_RELAY_30A_X2_V1.1` |
| **Microcontrolador** | ESP32-WROOM-32 / ESP32-WROOM-32E (Dual Core 32-bit LX6 @ 240 MHz, 4MB Flash, 520KB SRAM) |
| **Relés de Potência** | **2 Canais de 30A** (250V AC / 30V DC) com isolamento óptico e contatos COM, NO, NC |
| **LED Onboard** | Indicador de Status no **GPIO 5** |
| **Relé 1 (30A - K1)** | Acionamento de Potência no **GPIO 13** |
| **Relé 2 (30A - K2)** | Acionamento de Potência no **GPIO 12** |
| **Botão de Usuário / Boot** | Entrada com resistor Pull-Up no **GPIO 0 (IO0)** |
| **Entrada de Alimentação** | Borne DC (7V a 30V DC) ou Conector Micro-USB (5V DC) |
| **Porta Micro-USB** | **Exclusiva para alimentação elétrica** (a placa não possui conversor USB-Serial integrado) |
| **Interface de Gravação / Programação** | Header UART Serial de 6 pinos (TXD, RXD, IO0, EN/RST, GND, 5V/3V3) |

---

## 3. Mapeamento de Pinos (Pinout Oficial)

| Periférico / Pino | GPIO (ESP32) | Direção Padrão | Nível / Comportamento | Descrição |
| :--- | :---: | :---: | :---: | :--- |
| 🔵 **LED Onboard** | **`GPIO 5`** | OUTPUT | Active **HIGH** | LED indicador de status / atividade da placa |
| 🟢 **Relé 1 de Potência (K1)** | **`GPIO 13`** | OUTPUT | Active **HIGH** | Aciona o optoacoplador e bobina do Relé 1 (30A) |
| 🟢 **Relé 2 de Potência (K2)** | **`GPIO 12`** | OUTPUT | Active **HIGH** | Aciona o optoacoplador e bobina do Relé 2 (30A) |
| 🟡 **Botão Físico (IO0 / S1)** | **`GPIO 0`** | INPUT_PULLUP | Active **LOW** | Botão de usuário e seleção do modo Bootloader |
| 🔌 **UART0 TX** | **`GPIO 1`** | UART | Transmissão | Comunicação Serial / Gravação de Firmware |
| 🔌 **UART0 RX** | **`GPIO 3`** | UART | Recepção | Comunicação Serial / Gravação de Firmware |
| ⚙️ **Grade de GPIOs de Expansão** | `2, 4, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33` | OUTPUT / INPUT | Configurável | Pinos digitais livres para expansão, testes e acionamento de periféricos |
| 📥 **Pinos Input-Only** | `34, 35` | INPUT | Apenas Entrada | Entradas analógicas/digitais dedicadas a sensores |

---

## 4. Recursos do Painel Web

O painel web roda localmente na memória Flash do ESP32 através de PROGMEM e é servido de forma assíncrona com atualização contínua de status via polling (800ms):

1. **Seção 1 - Teste Rápido dos Recursos Onboard**:
   - **LED Onboard (GPIO 5)**: Botão de alternância de estado (Ligar/Desligar) e botão de pulso rápido (*Piscar 500ms*).
   - **Relé 1 de Potência 30A (GPIO 13)**: Botão de acionamento contínuo (com troca de cores e estado Aberto/Fechado) e botão de *Pulso de 1s*.
   - **Relé 2 de Potência 30A (GPIO 12)**: Botão de acionamento contínuo independente e botão de *Pulso de 1s*.
   - **Botão Boot / IO0 (GPIO 0)**: Card interativo com monitoramento dinâmico em tempo real do estado físico do botão (*Pressionado / Solto*).

2. **Seção 2 - Mapeamento e Controle de Todas as GPIOs**:
   - Controle individual de saídas digitais em tempo real.
   - Botões globais de lote: **"Ligar Todas as Saídas"** e **"Desligar Todas"**.
   - Configuração de modo por pino através do ícone ⚙️ (**OUTPUT** ou **INPUT com Pull-Up**).
   - Proteção de hardware para os pinos `GPIO 34` e `GPIO 35` (bloqueados como Input-Only).

3. **Portal Captivo**:
   - Redirecionamento automático para a página de teste ao conectar à rede Wi-Fi `ROBOBUILDERS-RB2555`.

---

## 5. Documentação da API REST

A placa disponibiliza uma API REST leve em formato JSON para automação e testes remotos:

### `GET /api/status`
Retorna o tempo de atividade (*uptime*) em segundos e o estado atual de todos os pinos mapeados.
```json
{
  "uptime": 42,
  "pins": [
    { "pin": 5, "mode": 0, "val": 1 },
    { "pin": 13, "mode": 0, "val": 0 },
    { "pin": 12, "mode": 0, "val": 0 },
    { "pin": 0, "mode": 1, "val": 1 }
  ]
}
```

### `GET /api/toggle?pin=<numero_gpio>`
Inverte o nível lógico de uma saída configurada como OUTPUT.
- **Exemplo**: `GET /api/toggle?pin=13` (Alterna o Relé 1)
- **Exemplo**: `GET /api/toggle?pin=12` (Alterna o Relé 2)
- **Exemplo**: `GET /api/toggle?pin=5` (Alterna o LED Onboard)
- **Resposta**: `{"pin":13,"val":1}`

### `GET /api/pulse?pin=<numero_gpio>&ms=<duracao_ms>`
Gera um pulso em nível alto (`HIGH`) na GPIO selecionada pelo tempo informado (em milissegundos).
- **Exemplo**: `GET /api/pulse?pin=5&ms=500` (Pisca o LED por 500ms)
- **Exemplo**: `GET /api/pulse?pin=13&ms=1000` (Aciona o Relé 1 por 1 segundo)
- **Exemplo**: `GET /api/pulse?pin=12&ms=1000` (Aciona o Relé 2 por 1 segundo)
- **Resposta**: `{"success":true}`

### `GET /api/mode?pin=<numero_gpio>&mode=<0|1>`
Altera o modo de operação da GPIO (`0 = OUTPUT`, `1 = INPUT_PULLUP`).
- **Exemplo**: `GET /api/mode?pin=22&mode=1`

### `GET /api/all?state=<0|1>`
Altera simultaneamente todas as saídas configuradas como OUTPUT para nível lógico 1 (`HIGH`) ou 0 (`LOW`).
- **Exemplo**: `GET /api/all?state=1` (Liga todas as saídas)
- **Exemplo**: `GET /api/all?state=0` (Desliga todas as saídas)

---

## 6. Guia de Conexão e Gravação UART (Passo a Passo)

Como a placa **ESP-32_RELAY_30A_X2_V1.1** não possui chip conversor USB-Serial integrado, a gravação de firmware é realizada através do conector de barra de pinos UART utilizando um módulo conversor USB-Serial externo (**CP2102, FTDI FT232RL, CH340** ou **PL2303**).

### 1. Esquema de Ligação

```text
  +-------------------------+             +-------------------------------+
  |   Conversor USB-Serial  |             |  ESP-32_RELAY_30A_X2_V1.1     |
  |  (CP2102 / FTDI / CH340)|             |  (Header de 6 pinos UART)     |
  |                         |             |                               |
  |                     GND |<----------->| GND                           |
  |                TX / TXD |------------>| RX / RXD (GPIO 3)             |
  |                RX / RXD |<------------| TX / TXD (GPIO 1)             |
  |                5V / 3V3 |------------>| 5V / 3V3                      |
  +-------------------------+             +-------------------------------+
```

> ⚠️ **Aviso de Segurança**: **NUNCA** conecte o conversor USB-Serial à placa se a mesma estiver conectada à rede elétrica AC (110V/220V). Desconecte completamente a rede elétrica antes de programar.

---

### 2. Procedimento para Modo Bootloader (Gravação)

1. Conecte os 4 fios do conversor USB-Serial (GND, TX, RX, 5V).
2. Conecte temporariamente o pino **`IO0`** ao pino **`GND`** *(ou mantenha pressionado o botão físico `BOOT / IO0`)*.
3. Plugue o conversor USB no computador (ou pressione o botão **`EN / Reset`** com o `IO0` no GND).
4. Inicie o processo de upload via PlatformIO ou Arduino IDE.
5. Após o término da gravação, desconecte o `IO0` do `GND` e reinicie o ESP32.

---

## 7. Estrutura do Repositório

```text
RB2555/
├── include/
│   └── web_page.h              <- Interface Web completa em PROGMEM (HTML5, CSS3, JS Vanilla)
├── src/
│   └── main.cpp                <- Firmware principal PlatformIO (SoftAP, DNS, REST API, GPIOs)
├── ROBOBUILDERS_RB2555/        <- Sketch pronto para Arduino IDE
│   ├── ROBOBUILDERS_RB2555.ino
│   └── web_page.h
├── platformio.ini              <- Configurações de compilação, plataforma e bibliotecas
└── README.md                   <- Documentação técnica oficial do hardware e firmware
```

---

## 8. Como Compilar e Enviar

### Opção A: Via PlatformIO (VS Code ou CLI)
1. Abra a pasta do projeto no VS Code com o PlatformIO instalado.
2. Para compilar o firmware:
   ```bash
   pio run
   ```
3. Para gravar no ESP32:
   ```bash
   pio run --target upload
   ```
4. Para abrir o monitor serial (115200 bps):
   ```bash
   pio device monitor -b 115200
   ```

### Opção B: Via Arduino IDE
1. Abra o arquivo [`ROBOBUILDERS_RB2555.ino`](file:///C:/Users/Lucas/Desktop/ROBOBUILDERS/RB2555/Documentação%20da%20placa%20e%20projeto%20WEB/ROBOBUILDERS_RB2555/ROBOBUILDERS_RB2555.ino) localizado dentro da pasta `ROBOBUILDERS_RB2555`.
2. Em **Ferramentas > Placa**, selecione **ESP32 Dev Module**.
3. Instale a biblioteca **ArduinoJson** (versão 6.x) pelo Gerenciador de Bibliotecas.
4. Selecione a porta COM do seu conversor USB-Serial e clique em **Carregar**.

---

## 9. Acesso ao Painel Web

1. Alimente a placa **ROBOBUILDERS RB2555 (ESP-32_RELAY_30A_X2_V1.1)**.
2. No smartphone ou notebook, conecte-se à rede Wi-Fi criada pela placa:
   - **SSID**: `ROBOBUILDERS-RB2555`
   - **Senha**: *(Rede Aberta)*
3. Abra o navegador de internet e acesse:
   - **URL**: `http://192.168.4.1`
