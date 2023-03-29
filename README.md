# ESP32-C6 test

This is a test for dynamically loading code on the ESP32-C6.
The current implementation works for 32-bit and 64-bit RISC-V CPUs.

## Prerequisites

1. `apt install gcc-riscv64-unknown-elf`
2. `make prepare`

## Put it on your ESP32-C6

`make flash`
