


	# Call `func` (reg a0) in user mode.
	.global userCall
	.type userCall, %function
	.text
	.align 2
userCall:
	# Push return address to stack.
	addi sp, sp, -8
	sw ra, 0(sp)
	sw a0, 4(sp)
	
	# Set privilege to user mode.
	li a0, 0
	ecall
	
	# Call function.
	lw a0, 4(sp)
	jalr ra, a0
	
	# Set privilege to machine mode.
	li a0, 3
	ecall
	
	# Pop return address from stack.
	lw ra, 0(sp)
	addi sp, sp, 8
	
	# Return.
	jalr x0, ra
