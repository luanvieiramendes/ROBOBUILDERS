# 📘 DOCUMENTAÇÃO TÉCNICA: ROBOBUILDERS RB2554
### *Firmware de Diagnóstico e Controle de Hardware • ESP32 Relay 30A X4 V1.1 (`ESP-32_RELAY_30A_X4_V1.1`)*

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

O **ROBOBUILDERS RB2554** é o firmware de fábrica (*Board Diagnostic / Hardware Validation*) projetado especificamente para a placa **`ESP-32_RELAY_30A_X4_V1.1`** (equipada com o módulo **ESP32-WROOM-32E / ESP32-WROOM-32**).

O firmware disponibiliza um **Ponto de Acesso Wi-Fi autônomo** (`ROBOBUILDERS-RB2554`) com **Portal Captivo** e uma interface web moderna em **Modo Escuro (Dark Mode)**, permitindo validar e acionar os **4 canais de relés de potência de 30A**, o LED onboard, o botão de boot e barramentos de GPIO diretamente pelo celular ou computador sem necessidade de internet ou rede Wi-Fi externa.

---

## 2. Especificações de Hardware

| Item | Especificação |
| :--- | :--- |
| **Identificador do Produto** | ROBOBUILDERS RB2554 |
| **Modelo da Placa Base** | `ESP-32_RELAY_30A_X4_V1.1` |
| **Microcontrolador** | ESP32-WROOM-32E (Dual Core Xtensa® 32-bit LX6 @ 240 MHz, 4MB Flash, 520KB SRAM) |
| **Relés de Potência** | **4 Canais de 30A** (250V AC / 30V DC) com isolamento óptico e contatos COM, NO, NC |
| **LED Onboard** | Indicador de Status no **GPIO 5** |
| **Entrada de Alimentação** | Borne AC (90V a 250V AC) ou Borne DC (7V a 30V DC / 5V DC) |
| **Interface de Programação** | Header UART Serial de 6 pinos (TXD, RXD, IO0, EN/RST, GND, 5V/3V3) |

---

## 3. Mapeamento de Pinos (Pinout Oficial)

| Periférico / Pino | GPIO (ESP32) | Direção Padrão | Nível / Lógica | Descrição |
| :--- | :---: | :---: | :---: | :--- |
| 💡 **LED Onboard** | **`GPIO 5`** | OUTPUT | Active **HIGH** | LED indicador de status / atividade onboard |
| ⚡ **Relé 1 (30A)** | **`GPIO 12`** | OUTPUT | Active **HIGH** | Aciona a bobina e optoacoplador do Relé 1 (30A) |
| ⚡ **Relé 2 (30A)** | **`GPIO 13`** | OUTPUT | Active **HIGH** | Aciona a bobina e optoacoplador do Relé 2 (30A) |
| ⚡ **Relé 3 (30A)** | **`GPIO 14`** | OUTPUT | Active **HIGH** | Aciona a bobina e optoacoplador do Relé 3 (30A) |
| ⚡ **Relé 4 (30A)** | **`GPIO 15`** | OUTPUT | Active **HIGH** | Aciona a bobina e optoacoplador do Relé 4 (30A) |
| 🔘 **Botão Físico (IO0)** | **`GPIO 0`** | INPUT_PULLUP | Active **LOW** | Botão de usuário e seleção de Bootloader |
| 🔌 **UART0 TX** | **`GPIO 1`** | UART | Transmissão | Comunicação Serial / Gravação de Firmware |
| 🔌 **UART0 RX** | **`GPIO 3`** | UART | Recepção | Comunicação Serial / Gravação de Firmware |
| ⚙️ **Grade de GPIOs** | `2, 4, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33` | OUTPUT / INPUT | Configurável | Pinos digitais para expansão, atuadores e sensores |
| 📥 **Pinos Input-Only** | `34, 35` | INPUT | Apenas Entrada | Entradas analógicas (ADC) e sensores digitais |

---

## 4. Recursos do Painel Web

O painel web roda localmente na memória Flash do ESP32 e é servido de forma assíncrona com design responsivo (Mobile-First):

1. **Seção 1 - Teste dos 4 Relés de Potência & Recursos Onboard**:
   - **4x Relés 30A (GPIOs 12, 13, 14, 15)**: Cards dedicados com botões de acionamento contínuo (com troca de cores e estado Fechado/Aberto) e botões de *Pulso de 1s*.
   - **Ações em Lote para Relés**: Botões rápidos *"⚡ Ligar Todos os Relés (1 a 4)"* e *"🛑 Desligar Todos os Relés"*.
   - **LED Onboard (GPIO 5)**: Botão para alternar estado (Ligar/Desligar) e botão de pulso rápido (*Piscar 500ms*).
   - **Botão Boot / IO0 (GPIO 0)**: Card interativo com monitoramento dinâmico do estado físico (*Pressionado / Solto*).

2. **Seção 2 - Mapeamento e Controle de Todas as GPIOs**:
   - Controle individual de saídas digitais em tempo real.
   - Botões globais de lote: **"Ligar Todas as Saídas"** e **"Desligar Todas"**.
   - Configuração de modo por pino através do ícone ⚙️ (**OUTPUT** ou **INPUT com Pull-Up**).
   - Proteção de hardware para os pinos `GPIO 34` e `GPIO 35` (bloqueados como Input-Only no ESP32).

3. **Portal Captivo**:
   - Redirecionamento automático ao conectar à rede Wi-Fi `ROBOBUILDERS-RB2554`.

---

## 5. Documentação da API REST

A placa expõe uma API REST leve em formato JSON:

### `GET /api/status`
Retorna o tempo de atividade (*uptime*) e o estado atual de todos os pinos mapeados.
```json
{
  "uptime": 42,
  "pins": [
    { "pin": 5, "mode": 0, "val": 0 },
    { "pin": 12, "mode": 0, "val": 1 },
    { "pin": 13, "mode": 0, "val": 0 },
    { "pin": 14, "mode": 0, "val": 0 },
    { "pin": 15, "mode": 0, "val": 0 },
    { "pin": 0, "mode": 1, "val": 1 }
  ]
}
```

### `GET /api/toggle?pin=<numero_gpio>`
Inverte o estado lógico de uma saída digital configurada.
- **Exemplo**: `GET /api/toggle?pin=12` (Alterna o Relé 1)
- **Resposta**: `{"pin":12,"val":1}`

### `GET /api/pulse?pin=<numero_gpio>&ms=<duracao_ms>`
Gera um pulso em nível alto (`HIGH`) na GPIO selecionada pelo tempo informado (em milissegundos).
- **Exemplo**: `GET /api/pulse?pin=5&ms=500` (Pisca o LED por 500ms)
- **Resposta**: `{"success":true}`

### `GET /api/relays?state=<0|1>`
Aciona ou desliga simultaneamente todos os 4 canais de relés (GPIOs 12, 13, 14 e 15).
- **Exemplo**: `GET /api/relays?state=1` (Liga todos os 4 relés)
- **Resposta**: `{"success":true}`

### `GET /api/mode?pin=<numero_gpio>&mode=<0|1>`
Altera o modo de operação da GPIO (`0 = OUTPUT`, `1 = INPUT_PULLUP`).
- **Exemplo**: `GET /api/mode?pin=22&mode=1`

### `GET /api/all?state=<0|1>`
Altera simultaneamente todas as saídas configuradas como OUTPUT para nível lógico 1 (`HIGH`) ou 0 (`LOW`).

---

## 6. Guia de Conexão e Gravação UART (Passo a Passo)

A placa **ESP-32_RELAY_30A_X4_V1.1** é gravada através do conector de barra de pinos UART utilizando um módulo conversor USB-Serial (como **CP2102**, **FTDI FT232RL**, **CH340** ou **PL2303**).

### 1. Esquema de Ligação

```text
  +-------------------------+             +-------------------------+
  |  Conversor USB-Serial   |             |   Placa RB2554 (UART)   |
  |  (CP2102 / FTDI / CH340)|             |  (ESP-32_RELAY_30A_X4)  |
  |                         |             |                         |
  |                     GND |<----------->| GND                     |
  |          TX / TXD (Out) |------------>| RX / RXD0 (GPIO 3)      |
  |           RX / RXD (In) |<------------| TX / TXD0 (GPIO 1)      |
  |                5V / 3V3 |------------>| 5V / 3V3                |
  +-------------------------+             +-------------------------+
```

> ⚠️ **Aviso Crítico de Segurança**: **NUNCA** conecte o conversor USB-Serial ou o cabo de dados ao computador enquanto a placa estiver ligada na rede elétrica AC (110V/220V). Desconecte totalmente a alimentação de alta tensão antes de plugar o programador serial.

---

### 2. Procedimento para Modo Bootloader

1. Conecte os pinos GND, TX, RX e 5V/3V3 entre o gravador e a placa.
2. Pressione e mantenha pressionado o botão **BOOT / IO0 (GPIO 0)** da placa.
3. Conecte o cabo USB ao computador (ou dê um toque no botão **EN / Reset** mantendo o **BOOT** pressionado por 2 segundos).
4. Solte o botão **BOOT**. O ESP32 entrará em modo de download UART.
5. Inicie o processo de upload via PlatformIO ou Arduino IDE.

---

## 7. Estrutura do Repositório

```text
RB2554/
├── include/
│   └── web_page.h              <- Interface Web completa em PROGMEM (HTML5, CSS3, JS Vanilla)
├── src/
│   └── main.cpp                <- Firmware principal PlatformIO (SoftAP, DNS, REST API, GPIOs)
├── ROBOBUILDERS_RB2554/        <- Sketch pronto para Arduino IDE
│   ├── ROBOBUILDERS_RB2554.ino
│   └── web_page.h
├── platformio.ini              <- Configurações de compilação PlatformIO
└── README.md                   <- Documentação técnica oficial do hardware e firmware
```

---

---

## Como Compilar e Gravar via PlatformIO CLI

O gerenciamento, compilação e gravação do projeto são feitos diretamente via **PlatformIO Core (CLI)** pelo terminal.

### 1. Compilar o Firmware
```bash
pio run
```

### 2. Gravar o Firmware na Placa
```bash
pio run -t upload
```

### 3. Especificar a Porta Serial Manualmente
```bash
pio run -t upload --upload-port COM3
```

### 4. Abrir o Monitor Serial
```bash
pio device monitor
```

### 5. Limpar Arquivos de Build
```bash
pio run -t clean
```

---

## Opção Alternativa: Arduino IDE

Se optar por utilizar a Arduino IDE:
1. Abra o arquivo ROBOBUILDERS_RB2554/ROBOBUILDERS_RB2554.ino.
2. Em **Ferramentas > Placa > ESP32 Arduino**, selecione **ESP32 Dev Module**.
3. Selecione a **Porta COM** correspondente ao seu conversor USB-Serial.
4. No **Gerenciador de Bibliotecas**, instale a biblioteca ArduinoJson (versão 6.x).
5. Clique em **Carregar (Upload)**.

---

## 9. Acesso ao Painel Web

1. Ligue a placa **ROBOBUILDERS RB2554**.
2. Conecte o celular ou computador na rede Wi-Fi gerada pela placa:
   - **SSID**: `ROBOBUILDERS-RB2554`
   - **Senha**: *(Rede Aberta, sem senha)*
3. Abra o navegador e acesse:
   - **URL**: `http://192.168.4.1`