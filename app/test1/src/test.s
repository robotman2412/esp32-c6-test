	.option pic
	.attribute arch, "rv32i2p0_m2p0_a2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.section .pltf.wrapper, "ax"
	.align	1
	.globl	wrapper
	.type	wrapper, @function
wrapper:
.LFB0:
	.cfi_startproc
	lw t0, __pltf_wrapper
	jr t0
	.cfi_endproc
.LFE0:
	.size	wrapper, .-wrapper

	.globl __pltf_wrapper
	.align 2
__pltf_wrapper:
	.word 0
