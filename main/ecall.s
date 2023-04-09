
	# Imported globals.
	.global interruptVector
	.global interruptHandler
	.global ecallHandler
	.global exitCookie



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



	# Handler function.
	.global ecallISR
	.type ecallISR, %function
	.text
	.align 2
ecallISR:
	mret
	
	# Preserve registers.
	csrw mscratch, t0
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
	sb t1, scratchMPP, t0
	
	# Modify return address.
	la t1, .restore
	csrw mepc, t1
	li t1, 0xc00
	csrr t2, mstatus
	or t1, t1, t2
	csrw mstatus, t1
	
	# Forward ISR.
	lw t1, interruptHandler
	bne t1, zero, .fwdIH
	j .fwdIV
	
.fwdIH:
	# Forward (`interuptHandler`)
	# lw t1, interruptHandler
	jr t1
	
.fwdIV:
	# Forward (`interruptVector`)
	li t1, 0x7fffffff
	csrr t2, mcause
	and t2, t2, t1
	lw t1, interruptVector
	slli t2, t2, 2
	add t1, t2, t1
	lw t1, 0(t1)
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
