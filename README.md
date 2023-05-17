# ESP32-C6 test

This is a test for dynamically loading code on the ESP32-C6.
The current implementation works for RV32 ESP32s (ESP32-C series).

## Prerequisites

1. Install RISC-V GCC: `apt install g++-riscv64-linux-gnu gcc-riscv64-linux-gnu`
2. Prepare the workspace: `make prepare`

## Put it on your ESP32-C6

`make flash`

# Hardware
## SSD1306 display
| ESP32 pin | Display pin
| :-------- | :----------
| GPIO5     | SDA
| GPIO4     | SCL
