# Blink — ESP32-C6 Mini

Projeto PlatformIO que pisca os LEDs onboard do ESP32-C6 Mini.

## Pinos usados

| LED | GPIO | Observação |
| --- | --- | --- |
| LED simples (status) | GPIO15 | LED comum onboard |
| LED RGB WS2812 | GPIO8 | Controlado por `rgbLedWrite()` |

Se a sua placa usar outros pinos, altere `LED_PIN` e `RGB_LED_PIN` em
`platformio.ini` (seção `build_flags`).

## Compilar

```bash
pio run
```

## Gravar na placa (COM4)

```bash
pio run -t upload
```

A porta já está definida como `COM4` no `platformio.ini`.

## Monitor serial

```bash
pio device monitor
```

> O LED simples pisca a cada 500 ms; o RGB pisca em verde junto.

## Usar na Arduino IDE

O arquivo `blink_arduino/blink_arduino.ino` é uma versão autônoma do mesmo
sketch, separada de `src/main.cpp` para evitar duplicidade de `setup()` e
`loop()` ao abrir o projeto na Arduino IDE.

1. Abra `blink_arduino/blink_arduino.ino` na Arduino IDE.
2. Em **Ferramentas > Placa**, selecione **ESP32C6 Dev Module**
   (instale o pacote **esp32 by Espressif Systems**, versão 3.x ou superior).
3. Em **Ferramentas > Porta**, selecione **COM4**.
4. Se o monitor serial não mostrar nada, ative
   **Ferramentas > USB CDC On Boot > Enabled** e grave novamente.
5. Clique em **Carregar**.
