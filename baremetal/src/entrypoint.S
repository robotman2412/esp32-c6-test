
	.equ uart0_base, 0x60000000
	.equ uart0_tx_reg, uart0_base + 0x0000

	.global _raw_start
	.type _raw_start, %function
_raw_start:
	li t0, uart0_tx_reg
	li a0, 0x48
	sb a0, 0(t0)
	li a0, 0x69
	sb a0, 0(t0)
	li a0, 0x21
	sb a0, 0(t0)
	li a0, 0x0d
	sb a0, 0(t0)
	li a0, 0x0a
	sb a0, 0(t0)
halt:
	j halt
	
	# Dummy read-only data.
	.section ".rodata"
	.align 2
	.word 0xdeadbeef
	
	# Dummy read-write data.
	.section ".data"
	.align 2
	.word 0xf00dbabe
	
	# Dummy zero-init data.
	.section ".bss"
	.lcomm dummysymbol, 4
