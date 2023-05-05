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

#include "kernel.hpp"
#include <iostream>
#include <string.h>



// Interrupt vector table pointer.
size_t interruptPointer;
// Interrupt handler address.
size_t interruptHandler;



namespace kernel {

// Table of names for all integer registers.
const char *regnames[32] = {
	"X0",  "RA",  "SP",  "GP",  "TP",  "T0",  "T1",  "T2",
	"S0",  "S1",  "A0",  "A1",  "A2",  "A3",  "A4",  "A5",
	"A6",  "A7",  "S2",  "S3",  "S4",  "S5",  "S6",  "S7",
	"S8",  "S9",  "S10", "S11", "T3",  "T4",  "T5",  "T6",
};

typedef struct __attribute__((packed)) __attribute__((aligned(4))) {
	uint32_t opcode   : 7;
	uint32_t dest     : 5;
	uint32_t imm19_12 : 8;
	uint32_t imm11    : 1;
	uint32_t imm10_1  : 10;
	uint32_t imm20    : 1;
} inst_jal_t;
static_assert(sizeof(inst_jal_t) == 4, "inst_jal_t must be 4 bytes in size");

typedef struct __attribute__((packed)) __attribute__((aligned(4))) {
	uint32_t padd0    : 1;
	uint32_t imm10_1  : 10;
	uint32_t imm11    : 1;
	uint32_t imm19_12 : 8;
	uint32_t imm20    : 1;
	uint32_t signext  : 11;
} jal_off_t;
static_assert(sizeof(jal_off_t) == 4, "jal_off_t must be 4 bytes in size");

typedef union {
	inst_jal_t bits;
	uint32_t   raw;
} inst_jal_to_u32_t;

typedef union {
	jal_off_t bits;
	int32_t   raw;
} jal_off_to_u32_t;



// Does a register dump.
static void regdump(const riscv_regs_t *regs, unsigned long pc) {
	const int cols = 2;
	
	// Get some info.
	long mhartid;
	asm ("csrr %0, mhartid" : "=r" (mhartid));
	long mstatus;
	asm ("csrr %0, mstatus" : "=r" (mstatus));
	long mtvec;
	asm ("csrr %0, mtvec" : "=r" (mtvec));
	long mcause;
	asm ("csrr %0, mcause" : "=r" (mcause));
	long mscratch;
	asm ("csrr %0, mscratch" : "=r" (mscratch));
	long mepc;
	asm ("csrr %0, mepc" : "=r" (mepc));
	
	// Announce register dump.
	printf("Core %ld register dump:\n", mhartid);
	
	// Print PC first.
	printf("  PC : 0x%08lx  ", pc);
	// Then x1 through x31.
	for (size_t i = 1; i < 32; i++) {
		printf("  %-3s: 0x%08lx  ", regnames[i], regs->arr[i]);
		if (i % cols == cols - 1 || i == 31) fputc('\n', stdout);
	}
	
	printf("MHARTID:  0x%08lx\n", mhartid);
	printf("MSTATUS:  0x%08lx\n", mstatus);
	printf("MTVEC:    0x%08lx\n", mtvec);
	printf("MCAUSE:   0x%08lx\n", mcause);
	printf("MSCRATCH: 0x%08lx\n", mscratch);
	printf("MEPC:     0x%08lx\n", mepc);
}

} // namespace kernel

// ECALL handler.
// Returns 1 to return to machine mode, 0 to return to user mode.
bool ecallHandler(int syscall, int a1, int a2, int a3) {
	using namespace kernel;
	
	// Get context.
	ctx_t *ctx;
	asm ("csrr %0, mscratch" : "=r" (ctx));
	
	switch ((syscall_t) syscall) {
		default:
			// Unimplemented system call.
			std::cout << "Unimplemented system call 0x" << std::hex << syscall << '\n';
			if (ctx->is_super) {
				panic();
			} else {
				regdump(&ctx->u_regs, ctx->u_pc);
				std::cout << "Terminating process " << ctx->pid << '\n';
			}
			return 1;
		
		case syscall_t::SYS_ABICALL:
			if (ctx->is_super) goto priv;
			// ABI call.
			if (a1 >= 0 && a1 < ctx->u_abi_size) {
				std::cout << "ABI call #" << std::dec << a1 << '\n';
				ctx->is_super = 1;
				makeABICall(ctx, ctx->u_abi_table[a1]);
				ctx->is_super = 0;
				return 0;
			} else {
				std::cout << "Unimplemented ABI call #" << std::dec << a1 << '\n';
				if (ctx->is_super) {
					panic();
				} else {
					regdump(&ctx->u_regs, ctx->u_pc);
					std::cout << "Terminating process " << ctx->pid << '\n';
				}
				return 1;
			}
		
		case syscall_t::SYS_EXIT:
			if (ctx->is_super) goto priv;
			std::cout << "Process " << ctx->pid << " exited with code " << a1 << '\n';
			// Simply jump back to machine mode.
			return 1;
		
		case syscall_t::SYS_USERJUMP:
			if (!ctx->is_super) goto nopriv;
			std::cout << "Process " << ctx->pid << " starting\n";
			// Extremely simple jump to user mode.
			return 0;
		
		case syscall_t::SYS_USERENTER:
			if (!ctx->is_super) goto nopriv;
			// Create usermode context.
			memset(&ctx->u_regs, 0, sizeof(ctx->u_regs));
			ctx->u_pc      = a1;
			ctx->u_regs.sp = a2;
			ctx->u_regs.gp = ctx->m_regs.gp;
			ctx->u_regs.tp = ctx->m_regs.tp;
			// Have trap handler return to user mode.
			return 0;
	}
	
	// It jumps here when M-mode system call made from U-mode.
	nopriv:
	std::cout << "Process " << ctx->pid << " made M-mode system call 0x" << std::hex << syscall << " from U-mode\n";
	regdump(&ctx->u_regs, ctx->u_pc);
	std::cout << "Terminating process " << ctx->pid << '\n';
	return 1;
	
	priv:
	// It jumps here when U-mode system call made from M-mode.
	std::cout << "Kernel made U-mode system call 0x" << std::hex << syscall << " from M-mode\n";
	panic();
}

// Generic synchronous trap handler.
// Returns 1 to return to machine mode, 0 to return to user mode.
bool trapHandler() {
	using namespace kernel;
	
	// Check what received the trap.
	ctx_t *ctx;
	asm ("csrr %0, mscratch" : "=r" (ctx));
	size_t mcause;
	asm ("csrr %0, mcause" : "=r" (mcause));
	
	if (ctx->is_super) {
		// It was the kernel.
		std::cout << "Kernel received ";
	} else {
		// It was a process.
		std::cout << "Process " << std::dec << ctx->pid << " received ";
	}
	
	// Print trap code.
	switch (mcause) {
		default: std::cout << "unknown trap 0x" << std::hex << mcause; break;
		case 0:  std::cout << "instruction address misaligned"; break;
		case 1:  std::cout << "instruction access fault"; break;
		case 2:  std::cout << "illegal instruction"; break;
		case 3:  std::cout << "trace/breakpoint trap"; break;
		case 4:  std::cout << "load address misaligned"; break;
		case 5:  std::cout << "load access fault"; break;
		case 6:  std::cout << "store/AMO address misaligned"; break;
		case 7:  std::cout << "store/AMO access fault"; break;
		case 8:  std::cout << "ECALL from U-mode"; break;
		case 9:  std::cout << "ECALL from S-mode"; break;
		case 11: std::cout << "ECALL from M-mode"; break;
		case 12: std::cout << "instruction page fault"; break;
		case 13: std::cout << "load page fault"; break;
		case 15: std::cout << "store/AMO page fault"; break;
	}
	std::cout << '\n';
	
	// Decide what to do next.
	if (ctx->is_super) {
		// It was the kernel.
		panic();
	} else {
		// It was a process.
		regdump(&ctx->u_regs, ctx->u_pc);
	}
	
	// Return to M-mode.
	return 1;
}



namespace kernel {

// Get relative address from RISC-V JAL instruction.
static inline int32_t readJAL(uint32_t inst) {
	inst_jal_to_u32_t input  = { .raw = inst };
	jal_off_to_u32_t  output = {
		.bits = {
			.padd0    = 0,
			.imm10_1  = input.bits.imm10_1,
			.imm11    = input.bits.imm11,
			.imm19_12 = input.bits.imm19_12,
			.imm20    = input.bits.imm20,
			.signext  = (uint16_t) (input.bits.imm20 ? 0xffff : 0)
		}
	};
	return output.raw;
}

// Convert relative address to RISC-V JAL instruction.
static inline uint32_t writeJAL(int32_t offset) {
	jal_off_to_u32_t  input  = { .raw = offset };
	inst_jal_to_u32_t output = {
		.bits = {
			.opcode = 0x6f, // 1101111
			.dest   = 0,    // x0
			.imm19_12 = input.bits.imm19_12,
			.imm11    = input.bits.imm11,
			.imm10_1  = input.bits.imm10_1,
			.imm20    = input.bits.imm20,
		}
	};
	return output.raw;
}

// Do all generic setup required for user mode.
void init() {
	// Obtain initial vector setting.
	asm volatile ("csrr %0, mtvec" : "=r" (interruptPointer));
	std::cout << "CSR mtvec:         0x" << std::hex << interruptPointer << '\n';
	interruptPointer &= 0xfffffffc;
	
	// Read ISR address.
	uint32_t *instptr = (uint32_t *) interruptPointer;
	std::cout << "Interrupt handler: 0x" << std::hex << *instptr << '\n';
	interruptHandler = readJAL(*instptr) + interruptPointer;
	std::cout << "Interrupt handler: 0x" << std::hex << interruptHandler << '\n';
	
	// Copy customTrap to RAM.
	void *mem = malloc(customTrapSize);
	size_t offs = (size_t) mem - (size_t) &customTrap;
	std::cout << "Loaded ISR to 0x" << std::hex << (size_t) mem << " (offset 0x" << offs << ")\n";
	memcpy(mem, (const void*) &customTrap, customTrapSize);
	
	// Write custom ISR address.
	size_t isrptr = (size_t) mem;
	*instptr = writeJAL(isrptr - interruptPointer);
}

// Set active context.
void setCtx(ctx_t *ctx) {
	asm volatile ("csrw mscratch, %0" :: "r" (ctx));
	ctx->is_super = 1;
}

// Set active context to default context.
static ctx_t defualtCtx;
void setDefaultCtx() {
	setCtx(&defualtCtx);
}

// Get active context.
ctx_t *getCtx() {
	ctx_t *out;
	asm ("csrr %0, mscratch" : "=r" (out));
	return out;
}

// Critical failure.
void panic() {
	std::cout << "\n\n**** KERNEL PANIC ****\n\n";
	
	ctx_t *ctx;
	asm ("csrr %0, mscratch" : "=r" (ctx));
	std::cout << "Kernel regdump:\n";
	regdump(&ctx->m_regs, ctx->m_pc);
	std::cout << "\nProcess " << ctx->pid << " regdump:\n";
	regdump(&ctx->u_regs, ctx->u_pc);
	std::cout << '\n';
	
	std::cout << "SCRATCH:\n" << std::hex
		<< "  0x" << ctx->scratch[0] << '\n'
		<< "  0x" << ctx->scratch[1] << '\n'
		<< "  0x" << ctx->scratch[2] << '\n'
		<< "  0x" << ctx->scratch[3] << '\n'
		<< "  0x" << ctx->scratch[4] << '\n'
		<< "  0x" << ctx->scratch[5] << '\n'
		<< "  0x" << ctx->scratch[6] << '\n'
		<< "  0x" << ctx->scratch[7] << '\n';
	
	std::cout << "\n**** HALTED ****\n";
	asm volatile ("csrc mstatus, %0" :: "r" (8));
	while (1);
}

}
