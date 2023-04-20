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



	# ABI call wrapper.
	# This is a template for the function called from user mode to generate an ABI call.
	.global abiCallTemplate
	.global abiCallTemplateEnd
	.section ".rodata"
	.align 2
abiCallTemplate:
	# Preserve first two argument words.
	mv t0, a0
	mv t1, a1
	# Make system call.
	.align 2
	.option arch, -c
	li a0, sys_u_abicall
	auipc a1, 0
	lw a1, 16(a1)
	ecall
	# Return from this functor.
	jr ra
	.option arch, +c
	# Here, the system call number is appended.
abiCallTemplateEnd:
