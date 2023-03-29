
#pragma once

// SPARC
#define ELFLOADER_MACHINE_SPARC 0x02
// x86
#define ELFLOADER_MACHINE_X86 0x03
// Motorola 68000
#define ELFLOADER_MACHINE_M68000 0x04
// Motorola 88000
#define ELFLOADER_MACHINE_M88000 0x05
// MIPS
#define ELFLOADER_MACHINE_MIPS 0x08
// MIPS RS3000 Little-endian
#define ELFLOADER_MACHINE_RS3000 0x0A
// PowerPC
#define ELFLOADER_MACHINE_POWER32 0x14
// PowerPC (64-bit)
#define ELFLOADER_MACHINE_POWER64 0x15
// Arm (up to Armv7/AArch32)
#define ELFLOADER_MACHINE_ARM 0x28
// SPARC Version 9
#define ELFLOADER_MACHINE_SPARC9 0x2B
// IA-64
#define ELFLOADER_MACHINE_IA64 0x32
// STMicroelectronics ST100 processor
#define ELFLOADER_MACHINE_ST100 0x3C
// AMD x86-64
#define ELFLOADER_MACHINE_AMD64 0x3E
// Digital Equipment Corp. PDP-10
#define ELFLOADER_MACHINE_PDP10 0x40
// Digital Equipment Corp. PDP-11
#define ELFLOADER_MACHINE_PDP11 0x41
// STMicroelectronics ST9+ 8/16 bit microcontroller
#define ELFLOADER_MACHINE_ST9 0x43
// STMicroelectronics ST7 8-bit microcontroller
#define ELFLOADER_MACHINE_ST7 0x44
// Motorola MC68HC16 Microcontroller
#define ELFLOADER_MACHINE_MC68HC16 0x45
// Motorola MC68HC11 Microcontroller
#define ELFLOADER_MACHINE_MC68HC11 0x46
// Motorola MC68HC08 Microcontroller
#define ELFLOADER_MACHINE_MC68HC08 0x47
// Motorola MC68HC05 Microcontroller
#define ELFLOADER_MACHINE_MC68HC05 0x48
// STMicroelectronics ST19 8-bit microcontroller
#define ELFLOADER_MACHINE_ST19 0x4A
// Digital VAX
#define ELFLOADER_MACHINE_VAX 0x4B
// Arm 64-bits (Armv8/AArch64)
#define ELFLOADER_MACHINE_ARM64 0xB7
// Zilog Z80
#define ELFLOADER_MACHINE_Z80 0xDC
// RISC-V
#define ELFLOADER_MACHINE_RISCV 0xF3
// WDC 65C816
#define ELFLOADER_MACHINE_65816 0x101



// GCC architecture detection.
#ifdef __riscv
#define ELFLOADER_MACHINE ELFLOADER_MACHINE_RISCV
#else
#pragma message "\n\n\n\033[33mWarning: Unable to detect host architecture, cannot check OSABI value at run time.\033[0m\n\n\n"
#endif