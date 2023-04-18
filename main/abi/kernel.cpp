
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
}

} // namespace kernel

// ECALL handler.
// Returns whether the program should continue running.
bool ecallHandler(int syscall, int a1, int a2, int a3) {
	using namespace kernel;
	std::cout << "ECALL! (but i ignored it lol)\n";
	return true;
}

// Generic synchronous trap handler.
// Returns whether the program should continue running.
bool trapHandler() {
	using namespace kernel;
	
	// Check what received the trap.
	size_t mstatus;
	asm ("csrr %0, mstatus" : "=r" (mstatus));
	ctx_t *ctx;
	asm ("csrr %0, mscratch" : "=r" (ctx));
	size_t mcause;
	asm ("csrr %0, mcause" : "=r" (mcause));
	
	if (mstatus & 0x00001800) {
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
	if (mstatus & 0x00001800) {
		// It was the kernel.
		regdump(&ctx->m_regs, ctx->m_pc);
	} else {
		// It was a process.
		regdump(&ctx->u_regs, ctx->u_pc);
	}
	return false;
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

// Initialise a context.
void initCtx(ctx_t &ctx) {
	// Set x0 registers to 0 just in case.
	ctx.u_regs.arr[0] = 0;
	ctx.m_regs.arr[0] = 0;
}

// Set active context.
void setCtx(ctx_t *ctx) {
	asm volatile ("csrw mscratch, %0" :: "r" (ctx));
}

}
