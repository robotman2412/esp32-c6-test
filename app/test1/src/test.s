	.file	"main.cpp"
	.option nopic
	.attribute arch, "rv32i2p0_m2p0_a2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	1
	.globl	wrapper
	.type	wrapper, @function
wrapper:
.LFB0:
	.cfi_startproc
	lw t0, __plt+0
	jr t0
	ret
	.cfi_endproc
.LFE0:
	.size	wrapper, .-wrapper
