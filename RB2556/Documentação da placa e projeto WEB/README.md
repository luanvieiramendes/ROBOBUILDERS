# 📘 DOCUMENTAÇÃO TÉCNICA: ROBOBUILDERS RB2556
### *Firmware de Teste e Diagnóstico de Hardware • ESP32 Relay 30A X1 V1.1 (`esp32_relay_30a_x1_v1.1`)*

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

---

## 1. Visão Geral do Projeto

O **ROBOBUILDERS RB2556** é o firmware de fábrica (*Board Diagnostic / Hardware Validation*) projetado especificamente para a placa **`esp32_relay_30a_x1_v1.1`** (equipada com o módulo **ESP32-WROOM-32E**).

O firmware disponibiliza um **Ponto de Acesso Wi-Fi autônomo** com **Portal Captivo** e uma interface web moderna em **Modo Escuro (Dark Mode)**, permitindo validar o funcionamento de todos os componentes onboard e barramentos de GPIO diretamente pelo celular ou computador sem necessidade de internet.

---

## 2. Especificações de Hardware

| Item | Especificação |
| :--- | :--- |
| **Identificador do Produto** | ROBOBUILDERS RB2556 |
| **Modelo da Placa Base** | `esp32_relay_30a_x1_v1.1` |
| **Microcontrolador** | ESP32-WROOM-32E (Dual Core Xtensa® 32-bit LX6 @ 240 MHz, 4MB Flash, 520KB SRAM) |
| **Relé de Potência** | 1 Canal de 30A (250V AC / 30V DC) com isolamento óptico e contatos COM, NO, NC |
| **LED Onboard** | Indicador de Status no **GPIO 5** |
| **Entrada de Alimentação** | Borne DC (7V a 30V DC) ou Conector Micro-USB (5V DC) |
| **Nota sobre a Porta USB** | A porta Micro-USB integrada é **apenas para alimentação elétrica**. Não possui chip conversor USB-Serial integrado na placa. |
| **Interface de Programação** | Header UART Serial de 6 pinos (TXD, RXD, IO0, EN/RST, GND, 5V/3V3) |

---

## 3. Mapeamento de Pinos (Pinout Oficial)

| Periférico / Pino | GPIO (ESP32) | Direção Padrão | Nível / Comportamento | Descrição |
| :--- | :---: | :---: | :---: | :--- |
| 🔵 **LED Onboard** | **`GPIO 5`** | OUTPUT | Active **HIGH** | LED indicador de status / atividade onboard |
| 🟢 **Relé 30A Principal** | **`GPIO 16`** | OUTPUT | Active **HIGH** | Aciona o optoacoplador e bobina do relé de 30A |
| 🟡 **Botão Físico (IO0)** | **`GPIO 0`** | INPUT_PULLUP | Active **LOW** | Botão de usuário e seleção de Bootloader |
| 🔌 **UART0 TX** | **`GPIO 1`** | UART | Transmissão | Comunicação Serial / Gravação de Firmware |
| 🔌 **UART0 RX** | **`GPIO 3`** | UART | Recepção | Comunicação Serial / Gravação de Firmware |
| ⚙️ **Grade de GPIOs** | `2, 4, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33` | OUTPUT / INPUT | Configurável | Pinos digitais para expansão e acionamento de periféricos |
| 📥 **Pinos Input-Only** | `34, 35` | INPUT | Apenas Entrada | Entradas de sensores / leituras digitais e analógicas |

---

## 4. Recursos do Painel Web

O painel web roda localmente na memória Flash do ESP32 e é servido de forma assíncrona:

1. **Seção 1 - Teste Rápido Onboard**:
   - **LED Onboard (GPIO 5)**: Botão para alternar estado (Ligar/Desligar) e botão de pulso rápido (*Piscar 500ms*).
   - **Relé de Potência 30A (GPIO 16)**: Botão de acionamento contínuo (com troca de cores e estado Fechado/Aberto) e botão de *Pulso de 1s*.
   - **Botão Boot / IO0 (GPIO 0)**: Card interativo com monitoramento dinâmico do estado físico do botão (*Pressionado / Solto*).

2. **Seção 2 - Mapeamento e Controle de Todas as GPIOs**:
   - Controle individual de saídas digitais em tempo real.
   - Botões globais de lote: **"Ligar Todas as Saídas"** e **"Desligar Todas"**.
   - Configuração de modo por pino através do ícone ⚙️ (**OUTPUT** ou **INPUT com Pull-Up**).
   - Proteção de hardware para os pinos `GPIO 34` e `GPIO 35` (bloqueados como Input-Only).

3. **Portal Captivo**:
   - Redirecionamento automático ao conectar à rede Wi-Fi `ROBOBUILDERS-RB2556`.

---

## 5. Documentação da API REST

A placa expõe uma API REST leve em formato JSON:

### `GET /api/status`
Retorna o tempo de atividade (*uptime*) e o estado atual de todos os pinos mapeados.
```json
{
  "uptime": 42,
  "pins": [
    { "pin": 5, "mode": 0, "val": 1 },
    { "pin": 16, "mode": 0, "val": 0 },
    { "pin": 0, "mode": 1, "val": 1 }
  ]
}
```

### `GET /api/toggle?pin=<numero_gpio>`
Inverte o estado lógico de uma saída digital configurada.
- **Exemplo**: `GET /api/toggle?pin=16` (Alterna o Relé de 30A)
- **Resposta**: `{"pin":16,"val":1}`

### `GET /api/pulse?pin=<numero_gpio>&ms=<duracao_ms>`
Gera um pulso em nível alto (`HIGH`) na GPIO selecionada pelo tempo informado (em milissegundos).
- **Exemplo**: `GET /api/pulse?pin=5&ms=500` (Pisca o LED por 500ms)
- **Resposta**: `{"success":true}`

### `GET /api/mode?pin=<numero_gpio>&mode=<0|1>`
Altera o modo de operação da GPIO (`0 = OUTPUT`, `1 = INPUT_PULLUP`).
- **Exemplo**: `GET /api/mode?pin=22&mode=1`

### `GET /api/all?state=<0|1>`
Altera simultaneamente todas as saídas configuradas como OUTPUT para nível lógico 1 (`HIGH`) ou 0 (`LOW`).

---

## 6. Guia de Conexão e Gravação UART (Passo a Passo)

Como a placa **esp32_relay_30a_x1_v1.1** possui porta USB voltada apenas para alimentação, a gravação de firmware é realizada através do conector de barra de pinos UART utilizando um módulo conversor USB-Serial (como **CP2102**, **FTDI FT232RL**, **CH340** ou **PL2303**).

### 1. Esquema de Ligação

```text
  +-------------------------+             +-------------------------+
  |  Conversor USB-Serial   |             |   Placa RB2556 (UART)   |
  |  (CP2102 / FTDI / CH340)|             |  (esp32_relay_30a_x1)   |
  |                         |             |                         |
  |                     GND |<----------->| GND                     |
  |          TX / TXD (Out) |------------>| RX / RXD0 (GPIO 3)      |
  |           RX / RXD (In) |<------------| TX / TXD0 (GPIO 1)      |
  |                5V / 3V3 |------------>| 5V / 3V3                |
  +-------------------------+             +-------------------------+
```

> ⚠️ **Aviso de Segurança**: Nunca conecte o conversor USB-Serial à placa se a mesma estiver conectada à rede elétrica AC (110V/220V). Desconecte completamente a rede elétrica antes de programar.

---

### 2. Procedimento para Modo Bootloader

1. Conecte o conversor USB-Serial ao computador.
2. Pressione e mantenha pressionado o botão **BOOT / IO0 (GPIO 0)** da placa.
3. Conecte o cabo USB (ou pressione o botão **EN / Reset** com o BOOT pressionado).
4. Solte o botão **BOOT** após 2 segundos. O ESP32 estará pronto para receber o novo código.

---

## 7. Estrutura do Repositório

```text
RB2556/
├── include/
│   └── web_page.h              <- Interface Web completa em PROGMEM (HTML5, CSS3, JS Vanilla)
├── src/
│   └── main.cpp                <- Firmware principal PlatformIO (SoftAP, DNS, REST API, GPIOs)
├── ROBOBUILDERS_RB2556/        <- Sketch pronto para Arduino IDE
│   ├── ROBOBUILDERS_RB2556.ino
│   └── web_page.h
├── platformio.ini              <- Configurações de compilação, plataforma e bibliotecas
└── README.md                   <- Documentação técnica oficial do hardware e firmware
```

---

## Como Compilar e Gravar via PlatformIO CLI

O gerenciamento, compilação e gravação do projeto são feitos diretamente via **PlatformIO Core (CLI)** pelo terminal.

### 1. Compilar o Firmware
`ash
pio run
`

### 2. Gravar o Firmware na Placa
`ash
pio run -t upload
`

### 3. Especificar a Porta Serial Manualmente
`ash
pio run -t upload --upload-port COM3
`

### 4. Abrir o Monitor Serial
`ash
pio device monitor
`

### 5. Limpar Arquivos de Build
`ash
pio run -t clean
`

---

## Opção Alternativa: Arduino IDE

Se optar por utilizar a Arduino IDE:
1. Abra o arquivo ROBOBUILDERS_RB2556/ROBOBUILDERS_RB2556.ino.
2. Em **Ferramentas > Placa > ESP32 Arduino**, selecione **ESP32 Dev Module**.
3. Selecione a **Porta COM** correspondente ao seu conversor USB-Serial.
4. No **Gerenciador de Bibliotecas**, instale a biblioteca ArduinoJson (versão 6.x).
5. Clique em **Carregar (Upload)**.


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
1. Abra o arquivo [`ROBOBUILDERS_RB2556.ino`](ROBOBUILDERS_RB2556/ROBOBUILDERS_RB2556.ino) localizado dentro da pasta `ROBOBUILDERS_RB2556`.
2. Em **Ferramentas > Placa**, selecione **ESP32 Dev Module**.
3. Instale a biblioteca **ArduinoJson** (versão 6.x) pelo Gerenciador de Bibliotecas.
4. Selecione a porta COM e clique em **Carregar**.

---

## 🌐 Acesso ao Painel

1. Ligue a placa **ROBOBUILDERS RB2556**.
2. Conecte o celular ou notebook na rede:
   - **SSID**: `ROBOBUILDERS-RB2556`
   - **Senha**: *(Aberta)*
3. Acesse no navegador:
   - **URL**: `http://192.168.4.1`
