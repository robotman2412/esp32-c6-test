
	# Address of default interrupt handler.
	.global interruptHandler
	# ECALL handler function.
	.global ecallHandler
	# Trap handler function.
	.global trapHandler
	# Get registers storage for current thread.
	.global getRegisterStorage

// Whether to preserve callee-saved registers.
#define PRESERVE_CALLEE_SAVE
// Whether to forward non-ecall traps from U-mode.
// #define FORWARD_NON_ECALL_U_TRAPS
// Whether to forward non-ecall traps from M-mode.
#define FORWARD_NON_ECALL_M_TRAPS

	# Named register indicies in registers struct.
	.equ rs_ra,  4
	.equ rs_sp,  8
	.equ rs_gp,  12
	.equ rs_tp,  16
	.equ rs_t0,  20
	.equ rs_t1,  24
	.equ rs_t2,  28
	.equ rs_s0,  32
	.equ rs_s1,  36
	.equ rs_a0,  40
	.equ rs_a1,  44
	.equ rs_a2,  48
	.equ rs_a3,  52
	.equ rs_a4,  56
	.equ rs_a5,  60
	.equ rs_a6,  64
	.equ rs_a7,  68
	.equ rs_s2,  72
	.equ rs_s3,  76
	.equ rs_s4,  80
	.equ rs_s5,  84
	.equ rs_s6,  88
	.equ rs_s7,  92
	.equ rs_s8,  96
	.equ rs_s9,  100
	.equ rs_s10, 104
	.equ rs_s11, 108
	.equ rs_t3,  112
	.equ rs_t4,  116
	.equ rs_t5,  120
	.equ rs_t6,  124

	# Offset of u_reg in ctx_t.
	.equ ctx_u_reg, 0
	# Offset of m_reg in ctx_t.
	.equ ctx_m_reg, 128
	# Offset of u_pc in ctx_t.
	.equ ctx_u_pc,  288
	# Offset of u_pc in ctx_t.
	.equ ctx_m_pc,  292

	# Offset of scratch in ctx_t.
	.equ ctx_scratch_0, 256
	.equ ctx_scratch_1, 260
	.equ ctx_scratch_2, 264
	.equ ctx_scratch_3, 268
	.equ ctx_scratch_4, 272
	.equ ctx_scratch_5, 276
	.equ ctx_scratch_6, 280
	.equ ctx_scratch_7, 284



	# Trap handler forwarding function.
	.global customTrap
	.type customTrap, %function
	.text
	.align 2
	.option arch, -c
customTrap:
	# Preserve tempregs.
	csrrw t0, mscratch, t0
	sw t1, ctx_scratch_1(t0)
	sw t2, ctx_scratch_2(t0)
	sw t3, ctx_scratch_3(t0)
	csrrw t1, mscratch, t0
	sw t0, ctx_scratch_0(t0)
	
	# Jump to handler.
	auipc t1, 0
	lw t1, 12(t1)
	jr t1
	.word customTrap0
	.option arch, +c

	# Size of customTrap function.
	.global customTrapSize
	.align 2
customTrapSize:
	.word .-customTrap


	
	# Trap handler implementation.
	.type customTrap0, %function
	.text
	.align 2
customTrap0:
	# Tempregs already saved.
	
	# Determine which privilege level trapped.
	csrr t1, mstatus
	sw t1, ctx_scratch_4(t0)
	csrr t3, mepc
	sw t3, ctx_scratch_5(t0)
	li t2, 0x1800
	and t2, t2, t1
	beq t2, x0, .utrap
	# j .mtrap
	
.mtrap:
	# Preserve return address.
	sw t3, ctx_m_pc(t0)
	addi t3, t0, ctx_m_reg
	sw t3, ctx_scratch_6(t0)
	j .saveregs
	
.utrap:
	# Preserve return address.
	sw t3, ctx_u_pc(t0)
	addi t3, t0, ctx_u_reg
	sw t3, ctx_scratch_6(t0)
	# j .saveregs
	
.saveregs:
	# Transfer scratch data.
	lw t1, ctx_scratch_0(t0)
	sw t1, rs_t0(t3)
	lw t1, ctx_scratch_1(t0)
	sw t1, rs_t1(t3)
	lw t1, ctx_scratch_2(t0)
	sw t1, rs_t2(t3)
	lw t1, ctx_scratch_3(t0)
	sw t1, rs_t3(t3)
	
	# Preserve remaining registers.
	sw ra, rs_ra(t3)
	sw sp, rs_sp(t3)
	sw gp, rs_gp(t3)
	sw tp, rs_tp(t3)
#ifdef PRESERVE_CALLEE_SAVE
	sw s0, rs_s0(t3)
	sw s1, rs_s1(t3)
#endif
	sw a0, rs_a0(t3)
	sw a1, rs_a1(t3)
	sw a2, rs_a2(t3)
	sw a3, rs_a3(t3)
	sw a4, rs_a4(t3)
	sw a5, rs_a5(t3)
	sw a6, rs_a6(t3)
	sw a7, rs_a7(t3)
#ifdef PRESERVE_CALLEE_SAVE
	sw s2, rs_s2(t3)
	sw s3, rs_s3(t3)
	sw s4, rs_s4(t3)
	sw s5, rs_s5(t3)
	sw s6, rs_s6(t3)
	sw s7, rs_s7(t3)
	sw s8, rs_s8(t3)
	sw s9, rs_s9(t3)
	sw s10, rs_s10(t3)
	sw s11, rs_s11(t3)
#endif
	sw t4, rs_t4(t3)
	sw t5, rs_t5(t3)
	sw t6, rs_t6(t3)
	
	# Check for ecall.
	csrr t1, mcause
	li t2, 8
	beq t1, t2, .handleEcall
	li t2, 11
	beq t1, t2, .handleEcall
	
	# Forward trap.
	csrr t1, mstatus
	li t2, 0x1800
	and t1, t2, t1
	beq x0, t1, .uforward
	j .mforward
	
.handleEcall:
	# Handle environment call.
	lw t1, ctx_scratch_5(t0)
	addi t1, t1, 4
	sw t1, ctx_scratch_5(t0)
	jal ra, ecallHandler
	j .restore
	
.mforward:
#ifdef FORWARD_NON_ECALL_M_TRAPS
	# Modify return address.
	la t1, .restore
	csrw mepc, t1
	li t1, 0x1800
	csrs mstatus, t1
	
	# Forward ISR.
	lw t1, interruptHandler
	jr t1
#else
	# Handle trap.
	la ra, trapHandler
	jalr ra, ra
	j .restore
#endif
	
.uforward:
#ifdef FORWARD_NON_ECALL_U_TRAPS
	# Modify return address.
	la t1, .restore
	csrw mepc, t1
	li t1, 0x1800
	csrs mstatus, t1
	
	# Forward ISR.
	lw t1, interruptHandler
	jr t1
#else
	# Handle trap.
	la ra, trapHandler
	jalr ra, ra
	# j .restore
#endif
	
.restore:
	# Get context pointers again (they were in caller-save registers).
	csrr t0, mscratch
	lw t3, ctx_scratch_6(t0)
	
	# Restore return address.
	lw t1, ctx_scratch_5(t0)
	csrw mepc, t1
	lw t1, ctx_scratch_4(t0)
	csrw mstatus, t1
	
	# Restore registers.
	lw ra, rs_ra(t3)
	lw sp, rs_sp(t3)
	lw gp, rs_gp(t3)
	lw tp, rs_tp(t3)
#ifdef PRESERVE_CALLEE_SAVE
	lw s0, rs_s0(t3)
	lw s1, rs_s1(t3)
#endif
	lw a0, rs_a0(t3)
	lw a1, rs_a1(t3)
	lw a2, rs_a2(t3)
	lw a3, rs_a3(t3)
	lw a4, rs_a4(t3)
	lw a5, rs_a5(t3)
	lw a6, rs_a6(t3)
	lw a7, rs_a7(t3)
#ifdef PRESERVE_CALLEE_SAVE
	lw s2, rs_s2(t3)
	lw s3, rs_s3(t3)
	lw s4, rs_s4(t3)
	lw s5, rs_s5(t3)
	lw s6, rs_s6(t3)
	lw s7, rs_s7(t3)
	lw s8, rs_s8(t3)
	lw s9, rs_s9(t3)
	lw s10, rs_s10(t3)
	lw s11, rs_s11(t3)
#endif
	lw t4, rs_t4(t3)
	lw t5, rs_t5(t3)
	lw t6, rs_t6(t3)
	
	# Restore tempregs.
	lw t1, ctx_scratch_0(t0)
	csrw mscratch, t1
	lw t3, ctx_scratch_3(t0)
	lw t2, ctx_scratch_2(t0)
	lw t1, ctx_scratch_1(t0)
	csrrs t0, mscratch, t0
	
	# Return from interrupt.
	mret
