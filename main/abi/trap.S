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

#include "kernel_include.S"



	# Address of default interrupt handler.
	.global interruptHandler
	# ECALL handler function.
	.global ecallHandler
	# Trap handler function.
	.global trapHandler



	# Trap handler forwarding function.
	.global customTrap
	.type customTrap, %function
	.text
	.align 2
customTrap:
	# Preserve tempregs.
	csrrw t0, mscratch, t0   # ctx in t0,  prev. t0 in mscratch
	sw t1, ctx_scratch_1(t0)
	sw t2, ctx_scratch_2(t0)
	sw t3, ctx_scratch_3(t0)
	csrrw t1, mscratch, t0   # prev. t0 in t1,  ctx in mscratch
	sw t1, ctx_scratch_0(t0)
	
	# Jump to handler.
	.option arch, -c
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
	lw t1, ctx_is_super(t0)
	beq t1, x0, .utrap
	
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
	#ifdef PRESERVE_GP
	sw gp, rs_gp(t3)
	#endif
	#ifdef PRESERVE_TP
	sw tp, rs_tp(t3)
	#endif
	sw s0, rs_s0(t3)
	sw s1, rs_s1(t3)
	sw a0, rs_a0(t3)
	sw a1, rs_a1(t3)
	sw a2, rs_a2(t3)
	sw a3, rs_a3(t3)
	sw a4, rs_a4(t3)
	sw a5, rs_a5(t3)
	sw a6, rs_a6(t3)
	sw a7, rs_a7(t3)
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
	sw t4, rs_t4(t3)
	sw t5, rs_t5(t3)
	sw t6, rs_t6(t3)
	
	# Load bare minimum machine regs.
	lw sp, ctx_m_reg_sp(t0)
	#ifdef PRESERVE_GP
	lw gp, ctx_m_reg_gp(t0)
	#endif
	#ifdef PRESERVE_TP
	lw tp, ctx_m_reg_tp(t0)
	#endif
	
	# Check for ECALL.
	csrr t1, mcause
	# Check for ECALL from U-mode.
	li t2, 8
	beq t1, t2, .uhandleEcall
	# Check for ECALL from M-mode.
	li t2, 11
	beq t1, t2, .mhandleEcall
	
	# Forward trap.
	lw t1, ctx_is_super(t0)
	beq x0, t1, .uforward
	j .mforward
	
.uhandleEcall:
	# Handle environment call from M-mode.
	lw t1, ctx_u_pc(t0)
	addi t1, t1, 4
	sw t1, ctx_u_pc(t0)
	sw t1, ctx_scratch_5(t0)
	jal ra, ecallHandler
	
	# Determine privilege mode to return to.
	csrr t0, mscratch
	bne x0, a0, .mrestore
	j .urestore
	
.mhandleEcall:
	# Handle environment call from M-mode.
	lw t1, ctx_m_pc(t0)
	addi t1, t1, 4
	sw t1, ctx_m_pc(t0)
	sw t1, ctx_scratch_5(t0)
	jal ra, ecallHandler
	
.checkRestore:
	# Determine privilege mode to return to.
	csrr t0, mscratch
	bne x0, a0, .mrestore
	# j .urestore
	
.urestore:
	# Set restore context to U-mode.
	addi t3, t0, ctx_u_reg
	#ifdef DUAL_M_MODE
	# Set MPP to 3.
	li t1, 0x00001800
	lw t2, ctx_scratch_4(t0)
	or t1, t2, t1
	sw t1, ctx_scratch_4(t0)
	#else
	# Set MPP to 0.
	li t1, 0xffffe7ff
	lw t2, ctx_scratch_4(t0)
	and t1, t2, t1
	sw t1, ctx_scratch_4(t0)
	#endif
	# Set to non-supervisor mode.
	li t1, 0
	sw t1, ctx_is_super(t0)
	# Set new PC.
	lw t1, ctx_u_pc(t0)
	sw t1, ctx_scratch_5(t0)
	# Restore context.
	j .restore
	
.mrestore:
	# Set restore context to M-mode.
	addi t3, t0, ctx_m_reg
	# Set MPP to 3.
	li t1, 0x00001800
	lw t2, ctx_scratch_4(t0)
	or t1, t2, t1
	sw t1, ctx_scratch_4(t0)
	# Set to supervisor mode.
	li t1, 1
	sw t1, ctx_is_super(t0)
	# Set new PC.
	lw t1, ctx_m_pc(t0)
	sw t1, ctx_scratch_5(t0)
	# Restore context.
	j .restore
	
.mforward:
#ifdef FORWARD_NON_ECALL_M_TRAPS
	# Modify return address.
	la t1, .mrestore
	csrw mepc, t1
	li t1, 0x1800
	csrs mstatus, t1
	
	# Forward ISR.
	lw t1, interruptHandler
	jr t1
#else
	# Handle trap.
	la ra, trapHandler
	li t1, 0x08
	csrs mstatus, t1
	jalr ra, ra
	li t1, 0x08
	csrc mstatus, t1
	j .checkRestore
#endif
	
.uforward:
#ifdef FORWARD_NON_ECALL_U_TRAPS
	# Modify return address.
	la t1, .urestore
	csrw mepc, t1
	li t1, 0x1800
	csrs mstatus, t1
	
	# Forward ISR.
	lw t1, interruptHandler
	jr t1
#else
	# Handle trap.
	la ra, trapHandler
	li t1, 0x08
	csrs mstatus, t1
	jalr ra, ra
	li t1, 0x08
	csrc mstatus, t1
	j .checkRestore
#endif
	
.restore:
	# Restore return address.
	lw t1, ctx_scratch_5(t0)
	csrw mepc, t1
	lw t1, ctx_scratch_4(t0)
	csrw mstatus, t1
	
	# Restore registers.
	lw ra, rs_ra(t3)
	lw sp, rs_sp(t3)
	#ifdef PRESERVE_GP
	lw gp, rs_gp(t3)
	#endif
	#ifdef PRESERVE_TP
	lw tp, rs_tp(t3)
	#endif
	lw s0, rs_s0(t3)
	lw s1, rs_s1(t3)
	lw a0, rs_a0(t3)
	lw a1, rs_a1(t3)
	lw a2, rs_a2(t3)
	lw a3, rs_a3(t3)
	lw a4, rs_a4(t3)
	lw a5, rs_a5(t3)
	lw a6, rs_a6(t3)
	lw a7, rs_a7(t3)
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
	lw t4, rs_t4(t3)
	lw t5, rs_t5(t3)
	lw t6, rs_t6(t3)
	
	lw t1, rs_t0(t3)
	sw t1, ctx_scratch_0(t0)
	lw t1, rs_t1(t3)
	sw t1, ctx_scratch_1(t0)
	lw t1, rs_t2(t3)
	sw t1, ctx_scratch_2(t0)
	lw t1, rs_t3(t3)
	sw t1, ctx_scratch_3(t0)
	
	# Enable interrupts.
	li t1, 0x80
	csrs mstatus, t1
	
	# Restore tempregs.
	lw t1, ctx_scratch_0(t0) # prev. t0 in t1
	csrw mscratch, t1        # prev. t0 in mscratch
	lw t3, ctx_scratch_3(t0)
	lw t2, ctx_scratch_2(t0)
	lw t1, ctx_scratch_1(t0)
	csrrw t0, mscratch, t0   # prev. to in t0,  ctx in mscratch
	
	# Return from trap.
	mret
