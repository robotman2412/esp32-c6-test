/*
	MIT License

	Copyright    (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files    (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifndef XLEN
#define XLEN 32
#endif

#ifndef PMP_SIZE
#define PMP_SIZE 16
#endif

namespace kernel {

// Table of names for all integer registers.
extern const char *regnames[32];

// Registers container for when some thread is not currently active.
union riscv_regs_t {
	// Unnamed RISCV registers.
	struct {
		long _x0; // Register x0 padding.
		long      x1,  x2,  x3,  x4,  x5,  x6,  x7;
		long x8,  x9,  x10, x11, x12, x13, x14, x15;
		long x16, x17, x18, x19, x20, x21, x22, x23;
		long x24, x25, x26, x27, x28, x29, x30, x31;
	};
	// Named RISCV registers.
	struct {
		long _zero; // Register x0 padding.
		long ra, sp, gp, tp;
		long t0, t1, t2;
		long s0, s1;
		long a0, a1, a2, a3, a4, a5, a6, a7;
		long s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
		long t3, t4, t5, t6;
	};
	// Array of register values.
	long arr[32];
};

}

extern "C" {

// Interrupt vector table pointer.
extern size_t interruptPointer;
// Interrupt handler address.
extern size_t interruptHandler;
// Trap handler.
void customTrap();
// Size of trap handler.
extern const size_t customTrapSize;

// ECALL handler.
// Returns whether the program should continue running.
bool ecallHandler(int syscall, int a1, int a2, int a3);
// Generic synchronous trap handler.
// Returns whether the program should continue running.
bool trapHandler();

} // extern "C"

namespace kernel {

// Storage for RISCV PMP entries.
struct riscv_pmp_t {
	union {
		// PMP configuration.
		uint8_t  cfg[PMP_SIZE];
		// Packed PMP configuration.
		unsigned long raw_cfg[PMP_SIZE/sizeof(unsigned long)];
	};
	// PMP addresses.
	unsigned long address[PMP_SIZE];
};

// Complete context for a usermode program.
// WARNING: This struct is directly referred to in assembly!
struct ctx_t {
	// Usermode: Registers storage.
	riscv_regs_t  u_regs;
	// Machine:  Registers storage.
	riscv_regs_t  m_regs;
	// Scratch data before ISR has read MPP.
	uint32_t      scratch[8];
	
	// Usermode: Program counter.
	unsigned long u_pc;
	// Machine:  Program counter.
	unsigned long m_pc;
	
	// Usermode: Upper stack bound.
	size_t u_stack_hi;
	// Usermode: Lower stack bound.
	size_t u_stack_lo;
	// Machine:  Upper stack bound.
	size_t m_stack_hi;
	// Machine:  Lower stack bound.
	size_t m_stack_lo;
	
	// Usermode: PMP settings.
	riscv_pmp_t   u_pmp;
	
	// Program ID.
	int pid;
};

static_assert(offsetof(ctx_t, u_regs) == 0, "offset of u_regs must be 0");
static_assert(offsetof(ctx_t, m_regs) == 128, "offset of m_regs must be 256");
static_assert(offsetof(ctx_t, scratch) == 256, "offset of scratch must be 256");
static_assert(offsetof(ctx_t, u_pc) == 288, "offset of u_pc must be 288");
static_assert(offsetof(ctx_t, m_pc) == 292, "offset of m_pc must be 292");

// Store all registers.
static inline void storeRegisters(riscv_regs_t *storage) __attribute__((always_inline));
static inline void storeRegisters(riscv_regs_t *storage) {
	asm volatile ("sw x1, %0" : "=m" (storage->x1));
	asm volatile ("sw x2, %0" : "=m" (storage->x2));
	asm volatile ("sw x3, %0" : "=m" (storage->x3));
	asm volatile ("sw x4, %0" : "=m" (storage->x4));
	asm volatile ("sw x5, %0" : "=m" (storage->x5));
	asm volatile ("sw x6, %0" : "=m" (storage->x6));
	asm volatile ("sw x7, %0" : "=m" (storage->x7));
	asm volatile ("sw x8, %0" : "=m" (storage->x8));
	asm volatile ("sw x9, %0" : "=m" (storage->x9));
	asm volatile ("sw x10, %0" : "=m" (storage->x10));
	asm volatile ("sw x11, %0" : "=m" (storage->x11));
	asm volatile ("sw x12, %0" : "=m" (storage->x12));
	asm volatile ("sw x13, %0" : "=m" (storage->x13));
	asm volatile ("sw x14, %0" : "=m" (storage->x14));
	asm volatile ("sw x15, %0" : "=m" (storage->x15));
	asm volatile ("sw x16, %0" : "=m" (storage->x16));
	asm volatile ("sw x17, %0" : "=m" (storage->x17));
	asm volatile ("sw x18, %0" : "=m" (storage->x18));
	asm volatile ("sw x19, %0" : "=m" (storage->x19));
	asm volatile ("sw x20, %0" : "=m" (storage->x20));
	asm volatile ("sw x21, %0" : "=m" (storage->x21));
	asm volatile ("sw x22, %0" : "=m" (storage->x22));
	asm volatile ("sw x23, %0" : "=m" (storage->x23));
	asm volatile ("sw x24, %0" : "=m" (storage->x24));
	asm volatile ("sw x25, %0" : "=m" (storage->x25));
	asm volatile ("sw x26, %0" : "=m" (storage->x26));
	asm volatile ("sw x27, %0" : "=m" (storage->x27));
	asm volatile ("sw x28, %0" : "=m" (storage->x28));
	asm volatile ("sw x29, %0" : "=m" (storage->x29));
	asm volatile ("sw x30, %0" : "=m" (storage->x30));
	asm volatile ("sw x31, %0" : "=m" (storage->x31));
}

// Machine mode instruction: Get current HART id.
static inline int mhartid() {
	int id;
	asm ("csrr %0, mhartid" : "=r" (id));
	return id;
}

// Do all generic setup required for user mode.
void init();
// Initialise a context.
void initCtx(ctx_t &ctx);
// Set active context.
void setCtx(ctx_t *ctx);

}
