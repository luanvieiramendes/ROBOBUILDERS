# 📘 DOCUMENTAÇÃO TÉCNICA: ESP-32_RELAY_30A_X8_V1.2
### *Firmware de Controle, Diagnóstico e Painel Web • ESP32 8 Canais de Relé 30A (`ESP-32_RELAY_30A_X8_V1.2`)*

---

## 📋 Sumário
- [1. Visão Geral do Projeto](#1-visão-geral-do-projeto)
- [2. Especificações de Hardware](#2-especificações-de-hardware)
- [3. Mapeamento de Pinos (Pinout Oficial)](#3-mapeamento-de-pinos-pinout-oficial)
- [4. Recursos do Painel Web](#4-recursos-do-painel-web)
- [5. Documentação da API REST](#5-documentação-da-api-rest)
- [6. Esquemas de Ligação Elétrica](#6-esquemas-de-ligação-elétrica)
- [7. Guia de Conexão e Gravação UART](#7-guia-de-conexão-e-gravação-uart)
- [8. Estrutura do Repositório](#8-estrutura-do-repositório)
- [9. Como Compilar e Enviar](#9-como-compilar-e-enviar)

---

## 1. Visão Geral do Projeto

Este projeto consiste no firmware completo de controle, diagnóstico e acionamento para a placa **`ESP-32_RELAY_30A_X8_V1.2`**, equipada com o módulo **ESP32-WROOM-32 / ESP32-WROOM-32E** e **8 relés de alta potência de 30A** optoisolados.

O firmware cria um **Ponto de Acesso Wi-Fi autônomo** (`ESP32-RELAY-30A-8CH`) com **Portal Captivo** e uma interface web moderna em **Dark Mode**, permitindo controlar individualmente cada um dos 8 relés, acionar pulsos temporizados, executar ações em lote (ligar/desligar todos), controlar o LED de status onboard, monitorar o botão físico BOOT e gerenciar GPIOs extras de expansão diretamente pelo navegador de qualquer smartphone ou computador.

---

## 2. Especificações de Hardware

| Item | Especificação |
| :--- | :--- |
| **Modelo da Placa** | `ESP-32_RELAY_30A_X8_V1.2` (ou `ESP-32_RELAY_X8`) |
| **Microcontrolador** | ESP32-WROOM-32 / ESP32-WROOM-32E (Dual Core 32-bit LX6 @ 240 MHz, 4MB Flash, 520KB SRAM) |
| **Canais de Relé** | **8 Relés de Alta Potência 30A** (250V AC / 30V DC por canal) |
| **Isolamento** | Acopladores ópticos (optoacopladores) dedicados para isolamento galvânico por canal |
| **Conexões dos Relés** | Bornes de parafuso industriais com saídas **COM** (Comum), **NO** (Normal Aberto) e **NC** (Normal Fechado) |
| **LED Onboard** | Indicador de Status onboard no **GPIO 5** |
| **Botão Onboard** | Botão físico BOOT / Usuário no **GPIO 0 (IO0)** |
| **Entrada de Alimentação** | Borne de alimentação DC (**7V a 30V DC**) ou conector **5V DC** |
| **Porta USB / Conexão** | A porta USB na placa é apenas para **alimentação elétrica**. Gravação de firmware é feita via header UART. |
| **Header de Programação** | Header UART Serial de pinos (TXD0, RXD0, IO0, EN/RST, GND, 3V3/5V) |

---

## 3. Mapeamento de Pinos (Pinout Oficial)

### A. 8 Canais de Relé 30A

| Canal do Relé | GPIO (ESP32) | Direção | Lógica de Acionamento | Descrição |
| :--- | :---: | :---: | :---: | :--- |
| ⚡ **Relé 1 (CH1)** | **`GPIO 32`** | `OUTPUT` | Active **HIGH** | Optoacoplador / Bobina do Relé 1 (30A) |
| ⚡ **Relé 2 (CH2)** | **`GPIO 33`** | `OUTPUT` | Active **HIGH** | Optoacoplador / Bobina do Relé 2 (30A) |
| ⚡ **Relé 3 (CH3)** | **`GPIO 25`** | `OUTPUT` | Active **HIGH** | Optoacoplador / Bobina do Relé 3 (30A) |
| ⚡ **Relé 4 (CH4)** | **`GPIO 26`** | `OUTPUT` | Active **HIGH** | Optoacoplador / Bobina do Relé 4 (30A) |
| ⚡ **Relé 5 (CH5)** | **`GPIO 27`** | `OUTPUT` | Active **HIGH** | Optoacoplador / Bobina do Relé 5 (30A) |
| ⚡ **Relé 6 (CH6)** | **`GPIO 14`** | `OUTPUT` | Active **HIGH** | Optoacoplador / Bobina do Relé 6 (30A) |
| ⚡ **Relé 7 (CH7)** | **`GPIO 12`** | `OUTPUT` | Active **HIGH** | Optoacoplador / Bobina do Relé 7 (30A) *(Strapping Pin)* |
| ⚡ **Relé 8 (CH8)** | **`GPIO 13`** | `OUTPUT` | Active **HIGH** | Optoacoplador / Bobina do Relé 8 (30A) |

### B. Recursos Onboard & Comunicação

| Periférico | GPIO (ESP32) | Direção | Nível / Modo | Descrição |
| :--- | :---: | :---: | :---: | :--- |
| 🔵 **LED Onboard** | **`GPIO 5`** | `OUTPUT` | Active **HIGH** | LED indicador de status / diagnóstico onboard |
| 🟡 **Botão Físico (IO0)** | **`GPIO 0`** | `INPUT_PULLUP` | Active **LOW** | Botão de usuário e seleção de Bootloader |
| 🔌 **UART0 TX** | **`GPIO 1`** | `UART` | Transmissão | Linha Serial TX para gravação / depuração |
| 🔌 **UART0 RX** | **`GPIO 3`** | `UART` | Recepção | Linha Serial RX para gravação / depuração |

### C. Barramento de GPIOs de Expansão

| Grupo | GPIOs | Modo Padrão | Observações |
| :--- | :---: | :---: | :--- |
| **I/O Configurável** | `2, 4, 15, 16, 17, 18, 19, 21, 22, 23` | `OUTPUT` | Pinos de expansão digital para sensores e atuadores |
| **Input-Only (Sensores)**| `34, 35` | `INPUT` | Entradas exclusivas para leitura digital/analógica de sensores |

---

## 4. Recursos do Painel Web

O painel web roda localmente na memória Flash do ESP32 via PROGMEM:

1. **Seção 1 - Controle dos 8 Relés de Potência 30A**:
   - 8 cards independentes com status visual em tempo real (**LIGADO / DESLIGADO**).
   - Botão **Ligar / Desligar** por canal com alteração dinâmica de cor.
   - Botão **Pulso 1s** para acionamentos temporizados rápidos.
   - Botões de lote: **"⚡ Ligar Todos os Relés"** e **"⛔ Desligar Todos os Relés"**.

2. **Seção 2 - Recursos Onboard**:
   - **LED Onboard (GPIO 5)**: Botão liga/desliga e função de teste de pulso (*Piscar 500ms*).
   - **Botão Boot / IO0 (GPIO 0)**: Card interativo com leitura dinâmica do estado do botão (*Pressionado / Solto*).

3. **Seção 3 - Barramento de Expansão & Demais GPIOs**:
   - Controle individual das saídas adicionais em tempo real.
   - Botões de lote para ligar/desligar todas as saídas de expansão.
   - Modal de configuração de modo por pino via ícone ⚙️ (**OUTPUT** ou **INPUT com Pull-Up**).
   - Proteção de hardware para os pinos `GPIO 34` e `GPIO 35` (Input-Only).

4. **Portal Captivo**:
   - Redirecionamento automático ao conectar à rede Wi-Fi `ESP32-RELAY-30A-8CH`.

---

## 5. Documentação da API REST

A placa expõe uma API REST JSON de alta performance:

### `GET /api/status`
Retorna o tempo de atividade (*uptime*) e o estado atual de todos os pinos e relés.
```json
{
  "uptime": 64,
  "pins": [
    { "pin": 32, "mode": 0, "val": 1 },
    { "pin": 33, "mode": 0, "val": 0 },
    { "pin": 5, "mode": 0, "val": 1 },
    { "pin": 0, "mode": 1, "val": 1 }
  ]
}
```

### `GET /api/toggle?pin=<numero_gpio>`
Inverte o estado lógico de um relé ou pino configurado como saída.
- **Exemplo**: `GET /api/toggle?pin=32` (Alterna o Relé 1)
- **Resposta**: `{"pin":32,"val":1}`

### `GET /api/pulse?pin=<numero_gpio>&ms=<duracao_ms>`
Gera um pulso em nível alto (`HIGH`) na GPIO selecionada pelo tempo informado (50ms a 10000ms).
- **Exemplo**: `GET /api/pulse?pin=5&ms=500` (Pisca o LED Onboard por 500ms)
- **Resposta**: `{"success":true}`

### `GET /api/relays?state=<0|1>`
Aciona ou desliga simultaneamente todos os **8 canais de relé**.
- **Exemplo**: `GET /api/relays?state=1` (Liga os 8 relés)
- **Resposta**: `{"success":true}`

### `GET /api/mode?pin=<numero_gpio>&mode=<0|1>`
Altera o modo de operação da GPIO (`0 = OUTPUT`, `1 = INPUT_PULLUP`).
- **Exemplo**: `GET /api/mode?pin=22&mode=1`

### `GET /api/all?state=<0|1>`
Altera o estado de todas as GPIOs configuradas como OUTPUT.

---

## 6. Esquemas de Ligação Elétrica

### A. Ligação das Cargas nos Relés de 30A (110V / 220V ou DC)

```text
       Rede Elétrica (ex: 110V / 220V AC)
           [ FASE ] --------------------+
                                        |
                                        v
                                 +--------------+
                                 |  COM (Comum) |
                                 |              |  Relé Canal 1..8
                                 |  NO (Aberto) |  (30A)
                                 +-------+------+
                                         |
                                         v
                                   [ + LÂMPADA / CARGA ]
                                   [ - LÂMPADA / CARGA ]
                                         |
           [ NEUTRO ] -------------------+
```

---

## 7. Guia de Conexão e Gravação UART

A placa **ESP-32_RELAY_30A_X8_V1.2** utiliza um header serial UART para gravação de firmware com conversores USB-Serial (como **CP2102**, **FTDI FT232RL**, **CH340** ou **PL2303**).

### 1. Esquema de Ligação

```text
  Conversor USB-Serial                           Placa ESP-32 Relay X8
 (CP2102 / FTDI / CH340)                               (Header UART)
   +-----------------+                               +---------------+
   |             GND |------------------------------>| GND           |
   |        TX / TXD |------------------------------>| RXD0 (GPIO 3) |
   |        RX / RXD |------------------------------>| TXD0 (GPIO 1) |
   |        5V / 3V3 |------------------------------>| 5V / 3V3      |
   +-----------------+                               +---------------+
```

### 2. Procedimento para Modo Bootloader

1. Conecte o conversor USB-Serial ao computador.
2. Pressione e mantenha pressionado o botão **BOOT / IO0 (GPIO 0)** da placa.
3. Pressione e solte o botão **EN / Reset** (ou reconecte a alimentação mantendo o BOOT pressionado).
4. Solte o botão **BOOT** após 2 segundos. O ESP32 estará no modo de gravação (*Download Boot*).

---

## 8. Estrutura do Repositório

```text
RB2553/
├── include/
│   └── web_page.h              <- Interface Web completa em PROGMEM (HTML5, CSS3, JS Vanilla)
├── src/
│   └── main.cpp                <- Firmware PlatformIO (SoftAP, DNS, REST API, 8 Relés de 30A)
├── ROBOBUILDERS_RB2556/        <- Sketch pronto para compilação na Arduino IDE
│   ├── ROBOBUILDERS_RB2556.ino
│   └── web_page.h
├── platformio.ini              <- Configurações de compilação do PlatformIO
└── README.md                   <- Documentação técnica oficial do hardware e pinout
```

---

## 9. Como Compilar e Enviar

### Opção A: Via PlatformIO (VS Code ou CLI)
1. Para compilar e gravar no ESP32 conectado:
   ```bash
   pio run --target upload
   ```
2. Para abrir o monitor serial (115200 bps):
   ```bash
   pio device monitor -b 115200
   ```

### Opção B: Via Arduino IDE
1. Abra o arquivo [`ROBOBUILDERS_RB2556.ino`](file:///C:/Users/Lucas/Desktop/ROBOBUILDERS/RB2553/Documentação%20da%20placa%20e%20projeto%20WEB/ROBOBUILDERS_RB2556/ROBOBUILDERS_RB2556.ino).
2. Em **Ferramentas > Placa**, selecione **ESP32 Dev Module**.
3. Selecione a porta COM e clique em **Carregar**.

---

## 🌐 Acesso ao Painel Web

1. Ligue a placa **ESP-32_RELAY_30A_X8_V1.2**.
2. Conecte seu dispositivo (celular ou computador) na rede Wi-Fi:
   - **SSID**: `ESP32-RELAY-30A-8CH`
   - **Senha**: *(Rede aberta / sem senha)*
3. Abra o navegador e acesse:
   - **URL**: **`http://192.168.4.1`**
