	.file	"main.cpp"
	.option nopic
	.attribute arch, "rv32i2p0_m2p0_a2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	1
	.globl	_start
	.type	_start, @function
_start:
.LFB0:
	.cfi_startproc
	li	a0,12
	ret
	.cfi_endproc
.LFE0:
	.size	_start, .-_start
	.align	1
	.globl	_Z6unusedv
	.type	_Z6unusedv, @function
_Z6unusedv:
.LFB1:
	.cfi_startproc
	li	a0,512
	ret
	.cfi_endproc
.LFE1:
	.size	_Z6unusedv, .-_Z6unusedv
	.align	1
	.globl	_Z4keptv
	.type	_Z4keptv, @function
_Z4keptv:
.LFB2:
	.cfi_startproc
	li	a0,1024
	ret
	.cfi_endproc
.LFE2:
	.size	_Z4keptv, .-_Z4keptv
	.ident	"GCC: () 10.2.0"
