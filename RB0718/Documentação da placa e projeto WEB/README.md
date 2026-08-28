# 📘 DOCUMENTAÇÃO TÉCNICA: ROBOBUILDERS RB0718
### *Firmware de Diagnóstico, Validação e Controle de Hardware • ESP32 Dev Kit Type-C*

---

## 📋 Sumário
- [1. Visão Geral do Projeto](#1-visão-geral-do-projeto)
- [2. Especificações de Hardware](#2-especificações-de-hardware)
- [3. Mapeamento de Pinos (Pinout Oficial)](#3-mapeamento-de-pinos-pinout-oficial)
- [4. Recursos do Painel Web](#4-recursos-do-painel-web)
- [5. Documentação da API REST](#5-documentação-da-api-rest)
- [6. Guia de Conexão e Gravação USB Type-C](#6-guia-de-conexão-e-gravação-usb-type-c)
- [7. Estrutura do Repositório](#7-estrutura-do-repositório)
- [8. Como Compilar e Enviar](#8-como-compilar-e-enviar)
- [9. Acesso ao Painel Web](#9-acesso-ao-painel-web)

---

## 1. Visão Geral do Projeto

O **ROBOBUILDERS RB0718** é o firmware de validação e diagnóstico de fábrica (*Board Diagnostic / Hardware Test*) para a placa de desenvolvimento **ESP32 Dev Kit (com conector USB Type-C)** equipada com o módulo **ESP32-WROOM-32**.

O firmware inicializa um **Ponto de Acesso Wi-Fi autônomo** (`ROBOBUILDERS-RB0718`) com **Portal Captivo** e uma interface web moderna em **Modo Escuro (Dark Mode)**, permitindo testar e controlar:
- O **LED Built-in** azul onboard (**GPIO 2**).
- O botão físico **BOOT / IO0** (**GPIO 0**).
- Todos os barramentos de pinos **GPIOs** digitais e de entrada analógica/sensores expostos no barramento da placa diretamente via navegador pelo smartphone ou computador, sem necessidade de aplicativo ou internet externa.

---

## 2. Especificações de Hardware

| Item | Especificação |
| :--- | :--- |
| **Identificador do Produto** | ROBOBUILDERS RB0718 |
| **Modelo da Placa Base** | ESP32 Dev Kit V1 / NodeMCU-32S (Type-C) |
| **Microcontrolador** | ESP32-WROOM-32 (Dual Core Xtensa® 32-bit LX6 @ 240 MHz, 4MB Flash, 520KB SRAM) |
| **Conector USB** | USB Type-C (Comunicação de dados e alimentação) |
| **Conversor USB-Serial** | CH340C / CP2102 onboard com auto-reset |
| **LED Onboard (Built-in)** | LED Azul conectado ao **`GPIO 2`** |
| **Botões Físicos** | Botão `BOOT / IO0` (GPIO 0) e Botão `EN / RST` (Reset de hardware) |
| **Tensão de Operação** | 3.3V DC (Níveis lógicos das GPIOs) |
| **Alimentação** | 5V via porta USB Type-C ou pino `VIN` (5V a 9V DC regulado para 3.3V) |

---

## 3. Mapeamento de Pinos (Pinout Oficial)

| Periférico / Pino | GPIO (ESP32) | Direção Padrão | Nível / Lógica | Descrição |
| :--- | :---: | :---: | :---: | :--- |
| 💡 **LED Built-in (Onboard)** | **`GPIO 2`** | OUTPUT | Active **HIGH** | LED Azul onboard indicador de atividade e status |
| 🔘 **Botão Físico BOOT** | **`GPIO 0`** | INPUT_PULLUP | Active **LOW** | Botão de usuário / Seleção de modo Bootloader |
| ⚙️ **GPIOs Digitais de E/S** | `4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33` | OUTPUT / INPUT | Configurável | Barramento de expansão digital para sensores e atuadores |
| 📥 **Pinos Input-Only (ADC)** | `34, 35, 36 (VP), 39 (VN)` | INPUT | Apenas Entrada | Pinos de leitura analógica/digital sem pull-up interno |
| 🔌 **UART0 TX / RX** | `GPIO 1 (TX) / GPIO 3 (RX)` | UART Serial | 115200 bps | Conectados internamente ao chip USB Type-C |

---

## 4. Recursos do Painel Web

O painel web é servido diretamente pela memória Flash do ESP32 via Wi-Fi SoftAP:

1. **Seção 1 - Recursos Onboard do DevKit**:
   - **LED Built-in (GPIO 2)**: Card interativo com botões para alternar estado (*Ligar/Desligar*), *Piscar 500ms* e *Pulso 1s*.
   - **Botão Boot (GPIO 0 / IO0)**: Monitoramento dinâmico em tempo real do estado (*Pressionado / Solto*).
   - **Diagnóstico do Módulo**: Exibição da arquitetura, clock da CPU (240MHz) e uso de memória RAM Heap livre.

2. **Seção 2 - Mapeamento e Controle de Todas as GPIOs**:
   - Controle individual de saídas digitais em tempo real.
   - Botões de ação em lote: **"⚡ Ligar Todas as Saídas"** e **"🛑 Desligar Todas as Saídas"**.
   - Configuração de modo por pino através do ícone ⚙️ (**OUTPUT** ou **INPUT com Pull-Up**).
   - Proteção de hardware para os pinos `GPIO 34, 35, 36 (VP) e 39 (VN)` (bloqueados como Input-Only no hardware ESP32).

3. **Portal Captivo Automático**:
   - Redirecionamento instantâneo ao conectar à rede Wi-Fi `ROBOBUILDERS-RB0718`.

---

## 5. Documentação da API REST

A placa disponibiliza uma API REST leve em JSON:

### `GET /api/status`
Retorna o tempo de atividade (*uptime*), memória heap livre e o estado de todos os pinos mapeados.
```json
{
  "uptime": 25,
  "free_heap": 234120,
  "chip_model": "ESP32-D0WDQ6",
  "cpu_freq": 240,
  "pins": [
    { "pin": 2, "mode": 0, "val": 1 },
    { "pin": 0, "mode": 1, "val": 1 },
    { "pin": 4, "mode": 0, "val": 0 }
  ]
}
```

### `GET /api/toggle?pin=<numero_gpio>`
Inverte o estado lógico de uma saída digital configurada (0 para 1 / 1 para 0).

### `GET /api/pulse?pin=<numero_gpio>&ms=<duracao_ms>`
Gera um pulso em nível HIGH pelo tempo especificado em milissegundos (ex: `ms=500`), retornando automaticamente para LOW.

### `GET /api/mode?pin=<numero_gpio>&mode=<0_ou_1>`
Altera o modo de operação do pino:
- `mode=0`: Saída Digital (**OUTPUT**)
- `mode=1`: Entrada Digital com Pull-Up (**INPUT_PULLUP**)

### `GET /api/all?state=<0_ou_1>`
Liga (`state=1`) ou desliga (`state=0`) simultaneamente todos os pinos configurados como saída digital.

---

## 6. Guia de Conexão e Gravação USB Type-C

1. Conecte a placa **RB0718 (ESP32 Dev Kit)** ao computador utilizando um **cabo USB Type-C de dados**.
2. Abra o **Gerenciador de Dispositivos** (Windows) e identifique a porta COM atribuída (exemplo: `COM3`, `COM4`).
3. O circuito de auto-reset da placa coloca o ESP32 em modo de gravação automaticamente. Se necessário em algum computador específico, mantenha pressionado o botão **BOOT** ao iniciar a gravação e solte após o início da transferência.

---

## 7. Estrutura do Repositório

```text
📁 Documentação da placa e projeto WEB/
│
├── 📁 ROBOBUILDERS_RB0718/          --> Projeto compatível com Arduino IDE
│   ├── ROBOBUILDERS_RB0718.ino      --> Código principal do firmware
│   └── web_page.h                   --> Interface Web HTML/CSS/JS (Dark Mode)
│
├── 📁 include/
│   └── web_page.h                   --> Header da página Web para PlatformIO
│
├── 📁 src/
│   └── main.cpp                     --> Código principal para PlatformIO
│
├── 📁 img/                          --> Diagramas e imagens de documentação
├── platformio.ini                    --> Arquivo de configuração do PlatformIO
└── README.md                        --> Documentação técnica completa
```

---

## 8. Como Compilar e Enviar

### Opção A: Usando VS Code + PlatformIO (Recomendado)
1. Abra a pasta do projeto no **VS Code**.
2. Verifique a porta COM no arquivo [platformio.ini](file:///c:/xampp/htdocs/ROBOBUILDERS/RB0718/Documenta%C3%A7%C3%A3o%20da%20placa%20e%20projeto%20WEB/platformio.ini) (`upload_port = COMx`).
3. Clique no ícone de **Upload (Seta para a direita)** na barra inferior do PlatformIO.

### Opção B: Usando Arduino IDE
1. Abra a pasta `ROBOBUILDERS_RB0718/` e abra o arquivo `ROBOBUILDERS_RB0718.ino`.
2. Em **Ferramentas > Placa**, selecione **ESP32 Dev Module**.
3. Selecione a **Porta COM** correspondente.
4. Instale a biblioteca `ArduinoJson` (versão 6.x) pelo Gerenciador de Bibliotecas.
5. Clique em **Carregar (Upload)**.

---

## 9. Acesso ao Painel Web

1. Após ligar o ESP32, conecte seu celular, tablet ou computador à rede Wi-Fi:
   - **Nome da Rede (SSID):** `ROBOBUILDERS-RB0718`
   - **Senha:** *(Rede Aberta, sem senha)*
2. O portal de conexão abrirá automaticamente. Caso não abra, acesse no navegador:
   - **URL / IP:** `http://192.168.4.1`
