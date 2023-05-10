PHDRS
{
	/* Text, readonly. */
	codeseg PT_LOAD;
	rodataseg PT_LOAD;
	dataseg PT_LOAD;
	/* Dynamic linking information */
	dynseg PT_DYNAMIC;
}

SECTIONS
{
	/* Origin is different than usual */
	. = 0x00010000;
	
	/* The usual ELF sections */
	.text   : { *(.text) *(.text*) } :codeseg
	.init_array : {
		/* Create symbols to keep track of `.init_array`. */
		__start_init_array = .;
		KEEP(*(.init_array))
		__stop_init_array = .;
	} :rodataseg
	.rodata : { *(.rodata) *(.rodata*) } :rodataseg
	.data   : { *(.data) *(.data*) } :dataseg
	.bss    : { *(.bss) *(.bss*) }
	
	/* Dynamic linking information. */
	.dynamic  : { KEEP(*(.dynamic)) } :dynseg :rodataseg
	.dynstr   : { KEEP(*(.dynstr)) }
	.rela.dyn : { KEEP(*(.rela.dyn)) }
}

/*
	Set the entry vector to _start.
	Note that in this ABI, `int _start(void)` is a function that returns the program's exit code.
	It is intended to be a wrapper around `int main(void)` to perform basic set-up.
*/
ENTRY(_start)
