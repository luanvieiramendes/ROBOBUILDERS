# 📘 DOCUMENTAÇÃO TÉCNICA: ROBOBUILDERS RB2557 (ESP32 HELLO WORLD TEST BOARD)
### *Ponto de Acesso Local Wi-Fi (SSID: ROBOBUILDERS-RB2557) • Teste de Hardware em Modo Escuro • Mapeamento Input/Output Configurável*

---

## 🎯 PROPÓSITO DO FIRMWARE

O firmware **ROBOBUILDERS RB2557** é o software de fábrica (*Hello World*) que acompanha a placa para testes e validações rápidas de todos os recursos de hardware.

- **Modo Escuro (Dark Mode)** de alto contraste e resposta em tempo real.
- **Acionamento de Cargas e Indicadores**: LED Onboard (GPIO 23) e Relé de Potência (GPIO 16).
- **Mapeamento Flexível de GPIOs**: Cada pino digital possui um ícone de engrenagem (⚙️) para configuração dinâmica entre **OUTPUT** (Saída Digital) e **INPUT** (Entrada Digital com Pull-Up).
- **Leitura do Botão Físico**: Leitura em tempo real do estado do botão BOOT (GPIO 0).

---

## 🔌 GUIA DE GRAVAÇÃO COM CONVERSOR USB-SERIAL (PASSO A PASSO)

Para gravar o firmware no ESP32 da placa **ROBOBUILDERS RB2557** utilizando um conversor USB-Serial externo (como CP2102, FT232RL/FTDI, CH340 ou PL2303), siga o procedimento detalhado abaixo.

### 1. Esquema de Ligação / Pinagem

Conecte os pinos do seu módulo conversor USB-Serial aos pinos correspondentes na placa RB2557:

| Conversor USB-Serial | Placa RB2557 (ESP32) | Observações |
| :--- | :--- | :--- |
| **GND** | **GND** | Obrigatório (Referência de terra comum) |
| **TXD (Transmissão)** | **RXD0 / GPIO 3** | Cruzado: TX do conversor vai no RX do ESP32 |
| **RXD (Recepção)** | **TXD0 / GPIO 1** | Cruzado: RX do conversor vai no TX do ESP32 |
| **5V / VCC** | **5V / VIN** | Alimentação da placa (ou use 3.3V no pino 3V3) |

> ⚠️ **Importante**: Certifique-se de que os níveis lógicos do conversor estejam em **3.3V** para proteger as entradas do ESP32.

---

### 2. Procedimento de Gravação (Modo Bootloader)

O ESP32 precisa entrar no modo de gravação (*Download Bootloader*) para receber o binário:

1. **Conecte o conversor USB-Serial** ao computador e à placa RB2557 (com o cabo desconectado ou alimentação desligada).
2. **Pressione e mantenha pressionado o botão BOOT (GPIO 0)** da placa RB2557.
3. **Ligue a alimentação da placa** (ou conecte o conversor USB na porta do computador) mantendo o botão **BOOT pressionado** por cerca de 2 segundos e depois solte-o.
   - *Alternativa*: Com a placa já ligada, segure **BOOT**, dê um clique rápido no botão **EN / RST (Reset)** e solte o **BOOT**.
4. No terminal ou IDE, execute o comando de compilação e upload:

```bash
# Gravação automática na porta detectada
pio run --target upload

# Ou especificando a porta COM (Exemplo: COM3)
pio run --target upload --upload-port COM3
```

5. O `esptool` detectará o chip (`Chip is ESP32-D0WD-V3`), gravará os blocos de memória e exibirá `[SUCCESS]`.

---

### 3. Reinicialização e Execução

Após o término da gravação com sucesso:

1. **Desligue e ligue a alimentação da placa** (Power Cycle) ou pressione o botão de **Reset (EN)**.
2. O ESP32 iniciará a execução normal do firmware, criando a rede Wi-Fi do ponto de acesso.

---

## 📌 MAPEAMENTO DE PINOS

| Componente | Pino Físico (GPIO) | Tipo Padrão | Descrição / Recursos |
| :--- | :--- | :--- | :--- |
| **LED Onboard** | **GPIO 23** | Saída Digital | Liga/Desliga e Piscar (500ms) |
| **Relé de Potência** | **GPIO 16** | Saída Digital | Acionamento direto e Pulso temporizado de 1s |
| **Botão Boot** | **GPIO 0** | Entrada Pull-Up | Leitura em tempo real (Pressionado / Solto) |
| **Grade de GPIOs** | **GPIOs 2, 4, 5, 12, 13, 14, 15, 17, 18, 19, 21, 22, 25, 26, 27, 32, 33** | Configurável (Output / Input) | Engrenagem ⚙️ para alternar entre Saída e Entrada |
| **Pinos de Entrada** | **GPIO 34, 35** | Entrada Digital | Pinos dedicados a entrada com leitura em tempo real |

---

## 🌐 COMO O USUÁRIO ACESSA O PAINEL WEB

1. Ligue a placa **ROBOBUILDERS RB2557**.
2. No celular, tablet ou computador, conecte na rede Wi-Fi:
   - **SSID**: `ROBOBUILDERS-RB2557`
   - **Senha**: *(Rede aberta, sem senha)*
3. Abra o navegador web e acesse:
   - **URL**: `http://192.168.4.1`
4. O painel em **Modo Escuro** carregará automaticamente com monitoramento e controle em tempo real.
