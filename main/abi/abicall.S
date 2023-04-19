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



	# ABI call implementation.
	# Context in `a0`, function pointer in `a1`.
	.global makeABICall
	.type makeABICall, %function
	.text
	.align 2
makeABICall:
	# Preserve the s* registers.
	addi sp, sp, -32
	sw s0, 16(sp)
	sw s1, 12(sp)
	sw s2, 8(sp)
	sw s3, 4(sp)
	sw s4, 0(sp)
	
	# Preserve registers in a slightly unusual location.
	mv s0, ra
	mv s1, sp
	mv s2, gp
	mv s3, tp
	
	# Rearrange arguments.
	mv ra, a1
	mv s4, a0
	lw a0, rs_t0(s4)
	lw a1, rs_t1(s4)
	lw a2, rs_a2(s4)
	lw a3, rs_a3(s4)
	lw a4, rs_a4(s4)
	lw a5, rs_a5(s4)
	lw a6, rs_a6(s4)
	lw a7, rs_a7(s4)
	
	# Perform call.
	jalr ra, 0(ra)
	
	# Store return value.
	sw a0, rs_a0(s4)
	sw a1, rs_a1(s4)
	
	# Restore registers.
	mv ra, s0
	mv sp, s1
	mv gp, s2
	mv tp, s3
	
	# Restore the s* registers.
	lw s0, 16(sp)
	lw s1, 12(sp)
	lw s2, 8(sp)
	lw s3, 4(sp)
	lw s4, 0(sp)
	addi sp, sp, 32
	
	# Return.
	ret
