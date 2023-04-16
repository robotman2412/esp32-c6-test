
	# Imported globals.
	.global interruptHandler



	# Scratch data.
	.data
	.align 2
scratchRet:
	.zero 4
scratchT0:
	.zero 4
scratchT1:
	.zero 4
scratchT2:
	.zero 4
scratchT3:
	.zero 4
scratchMPP:
	.zero 1



	# Handler forwarding device.
	.global customISR
	.type customISR, %function
	.text
	.align 2
customISR:
	csrw mscratch, t0
	auipc t0, 0
	lw t0, 12(t0)
	jalr x0, t0
	.word customISR0
	
	# Size of customISR function.
	.global customISRSize
	.align 2
customISRSize:
	.word .-customISR



	# Handler function.
	.type customISR0, %function
	.text
	.align 2
customISR0:
	# Preserve registers.
	# csrw mscratch, t0 # Done for us by the "redirector" function.
	sw t1, scratchT1, t0
	sw t2, scratchT2, t0
	sw t3, scratchT3, t0
	csrr t1, mscratch
	sw t1, scratchT0, t0
	
	# Preserve return address.
	csrr t1, mepc
	sw t1, scratchRet, t0
	csrr t1, mstatus
	srli t1, t1, 11
	andi t1, t1, 3
	sb t1, scratchMPP, t0
	
	# Check for ecall.
	csrr t1, mcause
	li t2, 8
	beq t1, t2, .handleEcall
	li t2, 11
	beq t1, t2, .handleEcall
	j .forward
	
.handleEcall:
	# Handle environment call.
	andi t1, a0, 3
	sw t1, scratchMPP, t0
	lw t1, scratchRet
	addi t1, t1, 4
	sw t1, scratchRet, t0
	j .restore
	
	# # Mask out MPP field from mstatus.
	# csrr t1, mstatus
	# li t2, 0xfffff3ff
	# and t1, t2, t1
	# # Insert new MPP value.
	# andi t2, a0, 3
	# slli t2, t2, 11
	# or t1, t2, t1
	# # Insert into mstatus.
	# csrw mstatus, t1
	# # Jump back.
	# csrr t1, mepc
	# addi t1, t1, 4
	# csrw mepc, t1
	# mret
	
.forward:
	# Modify return address.
	la t1, .restore
	csrw mepc, t1
	li t1, 0xc00
	csrr t2, mstatus
	or t1, t1, t2
	csrw mstatus, t1
	
	# Forward ISR.
	lw t1, interruptHandler
	jr t1
	
.restore:
	# Restore return address.
	lw t1, scratchRet
	csrw mepc, t1
	li t1, 0xfffff3ff
	csrr t2, mstatus
	and t2, t2, t1
	lb t1, scratchMPP
	slli t1, t1, 11
	or t2, t2, t1
	csrw mstatus, t2
	
	# Restore registers.
	lw t1, scratchT0
	csrw mscratch, t1
	lw t1, scratchT1
	lw t2, scratchT2
	lw t3, scratchT3
	csrr t0, mscratch
	
	# Return from interrupt.
	mret
