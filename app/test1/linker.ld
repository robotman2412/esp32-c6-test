SECTIONS
{
	/* Origin is different than usual */
	. = 0x00010000;
	
	/* Create symbols to keep track of `.init_array`. */
	.init_array : {
		__start_init_array = .;
		KEEP(*(.init_array))
		__stop_init_array = .;
	}
	
	/* Custom ELF sections */
	.pltf   : { *(.pltf) *(.pltf*) }
	
	/* The usual ELF sections */
	.text   : { *(.text) *(.text*) }
	.rodata : { *(.rodata) *(.rodata*) }
	.data   : { *(.data) *(.data*) }
	.bss    : { *(.bss) *(.bss*) }
}

/*
	Set the entry vector to _start.
	Note that in this ABI, `int _start(void)` is a function that returns the program's exit code.
	It is intended to be a wrapper around `int main(void)` to perform basic set-up.
*/
ENTRY(_start)
