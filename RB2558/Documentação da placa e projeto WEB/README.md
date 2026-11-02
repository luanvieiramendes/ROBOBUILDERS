# 📘 DOCUMENTAÇÃO TÉCNICA: ROBOBUILDERS RB2558 (`ESP32_Relay_X2`)
### *Ponto de Acesso Local Wi-Fi (SSID: ROBOBUILDERS-RB2558) • 2 Relés de Potência • Teste de Hardware em Modo Escuro • Mapeamento Input/Output Configurável*

---

## 🎯 PROPÓSITO DO FIRMWARE

O firmware **ROBOBUILDERS RB2558** é o software de fábrica (*Hello World*) desenvolvido para a placa **`ESP32_Relay_X2` (`303E32DC210`)**, acompanhando o módulo para testes e validações rápidas de todos os recursos de hardware em bancada.

- **Modo Escuro (Dark Mode)** de alto contraste e resposta em tempo real via Portal Captivo (`http://192.168.4.1`).
- **Acionamento de Cargas e Indicadores**: LED Onboard (**GPIO 23**), **Relé 1 - K1** (**GPIO 16**) e **Relé 2 - K2** (**GPIO 17**), com modos de liga/desliga e pulso temporizado.
- **Mapeamento Flexível de GPIOs**: Cada pino digital livre possui um ícone de engrenagem (⚙️) para configuração dinâmica entre **OUTPUT** (Saída Digital) e **INPUT** (Entrada Digital com Pull-Up).
- **Leitura do Botão Físico**: Leitura em tempo real do estado do botão **IO0 / BOOT** (**GPIO 0**).

---

## ⚙️ ESPECIFICAÇÕES DA PLACA (`ESP32_Relay_X2`)

| Item | Especificação |
| :--- | :--- |
| **Identificador do Produto** | ROBOBUILDERS RB2558 |
| **Modelo da Placa Base** | `ESP32_Relay_X2` (`303E32DC210`) |
| **Microcontrolador** | ESP32-WROOM-32 / ESP32-WROOM-32E (Dual Core 240 MHz, 4MB Flash, Wi-Fi + Bluetooth) |
| **Relés de Potência** | **2 Canais Independentes** Songle `SRD-05VDC-SL-C` (10A 250VAC / 10A 30VDC) |
| **Bornes dos Relés** | Canal 1 (`NO1`, `COM1`, `NC1`) e Canal 2 (`NO2`, `COM2`, `NC2`) |
| **Borne de Alimentação** | 3 vias (`VCC`, `GND`, `5V`) — Entrada **VCC** (7V a 30V DC) ou entrada direta **5V DC** |
| **Botões Físicos** | **EN** (Reset) e **IO0** (Bootloader / Entrada do Usuário no GPIO 0) |
| **Header de Gravação (`P4`)** | Barra de 6 pinos UART (`IO0`, `GND`, `GND`, `RX`, `TX`, `5V`) |

---

## 🔌 GUIA DE GRAVAÇÃO COM CONVERSOR USB-SERIAL (PASSO A PASSO)

Como a placa **ROBOBUILDERS RB2558 (`ESP32_Relay_X2`)** não possui chip conversor USB-Serial onboard, utilize um módulo conversor USB-Serial externo (como **CP2102, FT232RL/FTDI, CH340** ou **PL2303**) conectado ao barramento `P4` de 6 pinos para gravar o firmware.

### 1. Esquema de Ligação / Pinagem

Conecte os pinos do seu módulo conversor USB-Serial aos pinos correspondentes na placa RB2558:

| Conversor USB-Serial | Placa RB2558 (`ESP32_Relay_X2`) | Observações |
| :--- | :--- | :--- |
| **GND** | **GND** | Obrigatório (Referência de terra comum) |
| **TXD (Transmissão)** | **RX (RXD0 / GPIO 3)** | Cruzado: TX do conversor vai no RX do ESP32 |
| **RXD (Recepção)** | **TX (TXD0 / GPIO 1)** | Cruzado: RX do conversor vai no TX do ESP32 |
| **5V / VCC** | **5V** | Alimentação de 5V da placa e dos relés (ou 3.3V no pino 3V3) |

> ⚠️ **Importante**: Certifique-se de que os níveis lógicos de comunicação (`TX`/`RX`) do conversor estejam ajustados em **3.3V** para proteger as entradas do ESP32. Nunca conecte o conversor USB ao computador com cargas AC (110V/220V) energizadas nos relés.

---

### 2. Procedimento de Gravação (Modo Bootloader)

O ESP32 precisa entrar no modo de gravação (*Download Bootloader*) para receber o binário:

1. **Conecte o conversor USB-Serial** ao computador e à placa RB2558 (com o cabo desconectado ou alimentação desligada).
2. **Pressione e mantenha pressionado o botão `IO0` (BOOT / GPIO 0)** da placa RB2558 *(ou feche um jumper entre os pinos `IO0` e `GND` no conector `P4`)*.
3. **Ligue a alimentação da placa** (ou conecte o conversor USB na porta do computador) mantendo o botão **`IO0` pressionado** por cerca de 2 segundos e depois solte-o.
   - *Alternativa*: Com a placa já ligada, segure **`IO0`**, dê um clique rápido no botão **`EN` (Reset)** e solte o **`IO0`**.
4. No terminal ou IDE, execute o comando de compilação e upload:

```bash
# Gravação automática na porta detectada
pio run --target upload

# Ou especificando a porta COM (Exemplo: COM3)
pio run --target upload --upload-port COM3
```

5. O `esptool` detectará o chip ESP32, gravará os blocos de memória e exibirá `[SUCCESS]`.

---

### 3. Reinicialização e Execução

Após o término da gravação com sucesso:

1. Remova o jumper de `IO0` (caso tenha utilizado) e pressione o botão de **Reset (`EN`)** ou desligue e ligue a alimentação da placa (*Power Cycle*).
2. O ESP32 iniciará a execução normal do firmware, criando a rede Wi-Fi aberta **`ROBOBUILDERS-RB2558`** (`http://192.168.4.1`).

---

## 📌 MAPEAMENTO DE PINOS

| Componente | Pino Físico (GPIO) | Tipo Padrão | Descrição / Recursos |
| :--- | :--- | :--- | :--- |
| **LED Onboard** | **GPIO 23** | Saída Digital (Active HIGH) | Liga/Desliga e Piscar (500ms) |
| **Relé 1 de Potência (K1)** | **GPIO 16** | Saída Digital (Active HIGH) | Contatos `NO1/COM1/NC1` • Acionamento direto e Pulso de 1s |
| **Relé 2 de Potência (K2)** | **GPIO 17** | Saída Digital (Active HIGH) | Contatos `NO2/COM2/NC2` • Acionamento direto e Pulso de 1s |
| **Botão Boot (`IO0`)** | **GPIO 0** | Entrada Pull-Up (Active LOW) | Leitura em tempo real (Pressionado / Solto) |
| **Grade de GPIOs** | **GPIOs 2, 4, 5, 12, 13, 14, 15, 18, 19, 21, 22, 25, 26, 27, 32, 33** | Configurável (Output / Input) | Engrenagem ⚙️ para alternar entre Saída e Entrada |
| **Pinos de Entrada** | **GPIO 34, 35** | Entrada Digital | Pinos dedicados a entrada com leitura em tempo real |

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
1. Abra o arquivo `ROBOBUILDERS_RB2558/ROBOBUILDERS_RB2558.ino`.
2. Em **Ferramentas > Placa > ESP32 Arduino**, selecione **ESP32 Dev Module**.
3. Selecione a **Porta COM** correspondente ao seu conversor USB-Serial.
4. No **Gerenciador de Bibliotecas**, instale a biblioteca `ArduinoJson` (versão 6.x).
5. Clique em **Carregar (Upload)**.
