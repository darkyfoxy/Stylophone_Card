# STYLOPHONE CARD

This project shows a stylophone in credit card format.

I saw a project from [Mitxela](https://mitxela.com/projects/stylocard), and wanted to do this project.

Stylophone is based on [STM32L476RG](https://www.st.com/en/microcontrollers-microprocessors/stm32l476rg.html) (with FPU Arm Cortex-M4 MCU 80 MHz with 1 Mbyte of Flash memory). There is a built-in amplifier on [PAM8403](https://amperkot.ru/static/3236/uploads/datasheets/PAM8403_Datasheet.pdf).

There are also 4 user buttons, 4 user LEDs, a reset button and a power indicator LED.

You can see the [schematic](./Altium_Designer/Schematic.PDF).

[Demonstration video](https://www.youtube.com/watch?v=Ko7soT-JadY) ![youtube](https://img.shields.io/youtube/views/Ko7soT-JadY?style=social).

![](./img/card.png)


---

## Main features:

1. USB output. The PC recognizes the device as a microphone and can record sound.
2. USB input. The PC recognizes the device as a speaker and can play sound.
3. Audio output. You can connect an external device (AUDIO, GND).
4. Audio output with a built-in amplifier on [PAM8403](https://amperkot.ru/static/3236/uploads/datasheets/PAM8403_Datasheet.pdf). You can connect speakers (SP-, SP+).

I2C, UART, SPI not working.

There is also a BOOT0 jumper for DFU.

---

## Modes

Now there are 3 modes:

1. Guitar samples (button B1).
2. Square wave (button B2).
3. Sowtool wave (button B3).

There is also a vibrato effect (button B4).

Samples calculated with python in [jupyter notebook](./note_tables.ipynb).

---

## Acknowledgments

USB driver - https://github.com/dmitrii-rudnev/selenite-habr
