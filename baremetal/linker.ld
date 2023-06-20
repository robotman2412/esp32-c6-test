PHDRS
{
	codeseg   PT_LOAD;
	rodataseg PT_LOAD;
	dataseg   PT_LOAD;
	dynseg    PT_DYNAMIC;
}

SECTIONS
{
	/* Origin for read-only (flash) mapped bits. */
	. = 0x42000000;
	
	/* Code sections. */
	.text   : { *(.text) *(.text*) } :codeseg
	
	/* Read-only data sections. */
	.init_array : {
		/* Create symbols to keep track of `.init_array`. */
		__start_init_array = .;
		KEEP(*(.init_array))
		__stop_init_array = .;
	} :rodataseg
	.rodata : { *(.rodata) *(.rodata*) } :rodataseg
	
	/* Origin for read-write (RAM) mapped bits. */
	. = 0x40800000;
	__sram_start = .;
	
	/* Dynamic linking information. */
	.dynamic  : { KEEP(*(.dynamic)) } :dynseg :dataseg
	.dynstr   : { KEEP(*(.dynstr)) }
	.rela.dyn : { KEEP(*(.rela.dyn)) }
	
	/* Data sections. */
	.data   : { *(.data) *(.data*) } :dataseg
	.bss    : { *(.bss) *(.bss*) }
	
	/* End of static allocation. */
	__sram_static_end = .;
	__sram_static_size = __sram_static_end - __sram_start;
	__sram_end = 0x40880000;
}

/*
	Set the entry vector to _raw_start.
*/
ENTRY(_raw_start)
