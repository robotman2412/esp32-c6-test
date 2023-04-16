
#include <iostream>
#include <stdio.h>
// #include <ext/stdio_filebuf.h>

namespace std {

class fileptr_rdbuf: public std::streambuf {
	public:
		FILE *fd;
		fileptr_rdbuf(FILE *_fd): fd(_fd) {}
};

static streambuf *makeCoutBuf() {
	return new fileptr_rdbuf(stdout);
}

// istream cin(makeCinBuf());		/// Linked to standard input
// ostream cout(makeCoutBuf());		/// Linked to standard output
// ostream cerr;		/// Linked to standard error (unbuffered)
// ostream clog;		/// Linked to standard error (buffered)

// auto cinbuf = __gnu_cxx::stdio_filebuf<char>(fileno(stdin));

ios_base::Init::Init() {
}

ios_base::Init::~Init() {
}

};


/*

/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../libexec/gcc/riscv32-esp-elf/12.2.0/collect2
	-plugin
	/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../libexec/gcc/riscv32-esp-elf/12.2.0/liblto_plugin.so
	-plugin-opt=/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../libexec/gcc/riscv32-esp-elf/12.2.0/lto-wrapper
	-plugin-opt=-fresolution=/tmp/cc28cHp3.res
	-plugin-opt=-pass-through=-lgcc
	-plugin-opt=-pass-through=-lc
	-plugin-opt=-pass-through=-lnosys
	-plugin-opt=-pass-through=-lc
	-plugin-opt=-pass-through=-lgcc
	-melf32lriscv
	-o
	/tmp/lololol
	/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0/../../../../riscv32-esp-elf/lib/crt0.o
	/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0/crtbegin.o
	-L/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0
	-L/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc
	-L/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0/../../../../riscv32-esp-elf/lib
	--verbose
	/tmp/ccfyDiJd.o
	-lgcc
	-lc
	-lnosys
	-lc
	-lgcc
	/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0/crtend.o

/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../libexec/gcc/riscv32-esp-elf/12.2.0/collect2
	-plugin
	/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../libexec/gcc/riscv32-esp-elf/12.2.0/liblto_plugin.so
	-plugin-opt=/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../libexec/gcc/riscv32-esp-elf/12.2.0/lto-wrapper
	-plugin-opt=-fresolution=/tmp/cctm6hqi.res
	-plugin-opt=-pass-through=-lgcc
	-plugin-opt=-pass-through=-lc
	-plugin-opt=-pass-through=-lnosys
	-plugin-opt=-pass-through=-lc
	-plugin-opt=-pass-through=-lgcc
	-melf32lriscv
	-o
	/tmp/lololol
	/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0/../../../../riscv32-esp-elf/lib/crt0.o
	/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0/crtbegin.o
	-L/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0
	-L/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc
	-L/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0/../../../../riscv32-esp-elf/lib
	--verbose
	/tmp/ccwejtCq.o
	-lstdc++
	-lm
	-lgcc
	-lc
	-lnosys
	-lc
	-lgcc
	/home/julian/.espressif/tools/riscv32-esp-elf/esp-12.2.0_20230208/riscv32-esp-elf/bin/../lib/gcc/riscv32-esp-elf/12.2.0/crtend.o

*/
