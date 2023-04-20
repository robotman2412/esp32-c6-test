
#include <iostream>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdio.h>
#include <esp_log.h>

#include <elfloader.hpp>
#include <progloader.hpp>

#include <relocation.hpp>
#include <mpu.hpp>
#include <abi.hpp>

#include <kernel.hpp>

extern const char elf_start[] asm("_binary_main_o_start");
extern const char elf_end[] asm("_binary_main_o_end");

extern const char elf2_start[] asm("_binary_main2_o_start");
extern const char elf2_end[] asm("_binary_main2_o_end");

extern const char elflib_start[] asm("_binary_libtest_so_start");
extern const char elflib_end[] asm("_binary_libtest_so_end");



static kernel::ctx_t kernelCtx;

int cookie = 0;

void setTheCookie(int to) {
	std::cout << "Set cookie: " << std::dec << to << '\n';
	// for (int i = 0; i < 100000; i++) { asm volatile ("nop"); }
	cookie = to;
}

extern "C" void userCode() __attribute__((naked));
void userCode() {
	// Manual write of cookie variable.
	asm volatile("li t0, 1234"); /* New value of cookie */
	asm volatile("sw t0, cookie, t1"); /* Write to cookie variable using register t1 as address scratch */
	
	// Stall forever, but in assembly.
	// This causes the IWDT to run out,
	// which in turn resets the PRO CPU.
	asm volatile(".funny:\nj .funny");
	
	// ABI call write of cookie variable.
	asm volatile("li t0, 5678"); /* New value of cookie */
	asm volatile("li a0, 513"); /* Systemcall: Make ABI call */
	asm volatile("li a1, 0"); /* ABI call: function index (setTheCookie) */
	asm volatile("ecall"); /* Perform system call */
	
	// Exit.
	asm volatile("li a0, 512"); /* Systemcall: Exit */
	asm volatile("ecall");
}

extern "C" void goToMyUserCode() __attribute__((naked));
void goToMyUserCode(kernel::fptr_t functor) {
	asm volatile("mv a1, a0");
	asm volatile("li a0, 515");
	asm volatile("la a2, userStack");
	asm volatile("li t0, 4096");
	asm volatile("add a2, a2, t0");
	asm volatile("ecall");
	asm volatile("ret");
}

extern "C" uint32_t userStack[];
uint32_t userStack[1024];

extern "C" void app_main() {
	// Allow user-mode read-write-execute access to all of memory.
	mpu::appendRegion({
		0, 0x100000000,
		0,
		1, 1, 1,
		1
	});
	
	// Table of system calls.
	kernel::fptr_t abi[] = {
		(kernel::fptr_t) &setTheCookie
	};
	
	// Install custom trap handler.
	std::cout << "Preparing them kernels\n";
	kernel::init();
	kernel::setCtx(&kernelCtx);
	
	// Install table of system calls.
	kernelCtx.u_abi_table = abi;
	kernelCtx.u_abi_size  = sizeof(abi) / sizeof(kernel::fptr_t);
	std::cout << "Kernel ctx: 0x" << std::hex << (size_t) &kernelCtx << '\n';
	
	// Run the user-mode program.
	goToMyUserCode(userCode);
	std::cout << "Cookie: " << std::dec << cookie << '\n';
	
	return;
	
	
	/*
	kernel::init();
	kernel::setCtx(&kernelCtx);
	kernelCtx.u_abi_table = abi::getAbiTable();
	kernelCtx.u_abi_size  = abi::getAbiTableSize();
	
	loader::Linkage prog;
	abi::exportSymbols(prog.symbols);
	
	FILE *elf_fd = fmemopen((void *) elf2_start, elf2_end - elf2_start, "r");
	auto res = prog.loadExecutable(elf_fd);
	if (!res) return;
	
	auto regions = mpu::readRegions();
	for (const auto &region: regions) {
		std::cout << "Region:\n";
		std::cout << "  Base:   0x" << std::hex << region.base << '\n';
		std::cout << "  Size:   0x" << region.size << '\n';
		std::cout << "  RWX:    " << region.read << region.write << region.exec << '\n';
		std::cout << "  Active: " << region.active << "\n\n";
	}
	
	// Time to run prog.
	std::cout << "Running program!\n";
	goToMyUserCode((kernel::fptr_t) prog.entryFunc);
	std::cout << "Done!\n";
	// using EF = int(*)(int, const char **, const char**);
	// EF entry = (EF) prog.entryFunc;
	// const char *envarr[] = {
	// 	"EVNVVAR=1",
	// 	NULL
	// };
	// const char *lol = "the_program_lol";
	// int ec = entry(1, &lol, envarr);
	// std::cout << "Exit code 0x" << std::hex << ec << '\n';
	*/
}



/*
// void userCall(void (*callback)()) __attribute__((naked));
void userCall(void (*callback)()) {
	// Set exit point thingy.
	asm volatile ("");
	// returner:
	return;
}

extern "C" void app_main() {
	esp_log_level_set("elfloader", ESP_LOG_DEBUG);
	
	// prepUsermode();
	
	size_t mtvec;
	asm volatile ("csrr %0, mtvec" : "=r" (mtvec));
	std::cout << "CSR mtvec:       0x" << std::hex << mtvec << '\n';
	size_t mscratch;
	asm volatile ("csrr %0, mscratch" : "=r" (mscratch));
	std::cout << "CSR mscratch:    0x" << std::hex << mscratch << '\n';
	std::cout << "MPU granularity: 0x" << std::hex << mpu::granularity() << '\n';
	
	loader::Linkage prog;
	
	FILE *elflib_fd = fmemopen((void *) elflib_start, elflib_end - elflib_start, "r");
	bool res = prog.loadLibrary(elflib_fd);
	if (!res) return;
	
	FILE *elf_fd = fmemopen((void *) elf_start, elf_end - elf_start, "r");
	res = prog.loadExecutable(elf_fd);
	if (!res) return;
	
	auto regions = mpu::readRegions();
	for (const auto &region: regions) {
		std::cout << "Region:\n";
		std::cout << "  Base:   0x" << std::hex << region.base << '\n';
		std::cout << "  Size:   0x" << region.size << '\n';
		std::cout << "  RWX:    " << region.read << region.write << region.exec << '\n';
		std::cout << "  Active: " << region.active << "\n\n";
	}
	
	// Time to run prog.
	std::cout << "Running program!\n";
	using EF = int(*)(int(*)(const char*));
	EF entry = (EF) prog.entryFunc;
	int ec = entry(puts);
	std::cout << "Exit code 0x" << std::hex << ec << '\n';
}
*/



/*
int callback() {
	std::cout << "Callback!\n";
	return 32;
}

static size_t allocator(size_t vaddr, size_t len, size_t align) {
	return (size_t) malloc(len);
}

extern "C" void app_main() {
	bool res;
	// esp_log_level_set("elfloader", ESP_LOG_DEBUG);
	
	elf::SymMap map;
	
	// Open lib handle.
	FILE *elflib_fd = fmemopen((void *) elflib_start, elflib_end - elflib_start, "r");
	auto elflib = elf::ELFFile(elflib_fd);
	
	// Try to read lib.
	res = elflib.readDyn();
	std::cout << "Lib read:  " << res << '\n';
	if (!res) return;
	
	// Try to load lib.
	auto loadlib = elflib.load(allocator);
	res = !!loadlib;
	std::cout << "Lib load:  " << res << '\n';
	if (!res) return;
	
	// Try to link lib.
	res = elf::relocate(elflib, loadlib, map);
	std::cout << "Lib link:  " << res << '\n';
	if (!res) return;
	
	// Try to export lib.
	res = elf::exportSymbols(elflib, loadlib, map);
	std::cout << "Exp lib:   " << res << '\n';
	if (!res) return;
	
	
	// Open prog handle.
	FILE *elf_fd = fmemopen((void *) elf_start, elf_end - elf_start, "r");
	auto elf = elf::ELFFile(elf_fd);
	
	// Try to read lib.
	res = elf.readDyn();
	std::cout << "Prog read: " << res << '\n';
	if (!res) return;
	
	// Try to load lib.
	auto load = elf.load(allocator);
	res = !!load;
	std::cout << "Prog load: " << res << '\n';
	if (!res) return;
	
	// Try to link lib.
	res = elf::relocate(elf, load, map);
	std::cout << "Prog link: " << res << '\n';
	if (!res) return;
	
	
	// Time to run prog.
	std::cout << "Running program!\n";
	using MessagePrinter = void*(*)(const char *);
	using EF = int(MessagePrinter);
	EF *entry = (EF*) (elf.getHeader().entry + load.vaddr_offset());
	int ec = entry((MessagePrinter) puts);
	std::cout << "Exit code 0x" << std::hex << ec << '\n';
	
}
*/



/*
extern "C" void app_main() {
    std::cout << "LOLOLOL time!\n";
	
	FILE *fd = fmemopen((void *) elf_start, elf_end - elf_start, "r");
	elf::ELFFile myFile(fd);
	
	std::cout << "Header: " << myFile.isValid() << '\n';
	std::cout << "Sect:   " << myFile.readSect() << '\n';
	std::cout << "Prog:   " << myFile.readProg() << '\n';
	std::cout << "Sym:    " << myFile.readSym() << '\n';
	std::cout << "Dynsym: " << myFile.readDynSym() << '\n';
	std::cout << '\n';
	std::cout << '\n';
	
	const auto &header = myFile.getHeader();
	std::cout << "Header:\n";
	std::cout << "  OSABI: 0x" << std::hex << (int) header.osabi << '\n';
	std::cout << "  entry: 0x" << header.entry << '\n';
	std::cout << "  #prog: " << std::dec << header.phEntNum << "x sz=" << header.phEntSize << " @" << header.phOffset << '\n';
	std::cout << "  #sect: " << header.shEntNum << "x sz=" << header.shEntSize << " @" << header.shOffset << '\n';
	std::cout << '\n';
	std::cout << '\n';
	
	for (const auto &sect: myFile.getSect()) {
		if (!sect.name.size()) continue;
		std::cout << "Sect '" << sect.name << "':\n";
		std::cout << "  Type:  0x" << std::hex << sect.type << '\n';
		std::cout << "  Flags: 0x" << sect.flags << '\n';
		std::cout << "  Vaddr: 0x" << sect.vaddr << '\n';
		std::cout << "  Offs:  " << std::dec << sect.offset << '\n';
		std::cout << "  Size:  " << sect.file_size << '\n';
		std::cout << "  Align: " << sect.alignment << '\n';
		std::cout << '\n';
	}
	std::cout << '\n';
	
	for (const auto &prog: myFile.getProg()) {
		std::cout << "Prog:\n";
		std::cout << "  Type:  0x" << std::hex << prog.type << '\n';
		std::cout << "  Offs:  0x" << std::dec << prog.offset << '\n';
		std::cout << "  Vaddr: 0x" << std::hex << prog.vaddr << '\n';
		std::cout << "  Paddr: 0x" << prog.paddr << '\n';
		std::cout << "  Size:  " << std::dec << prog.file_size << '/' << prog.mem_size << '\n';
		std::cout << "  Flags: 0x" << std::hex << prog.flags << '\n';
		std::cout << std::dec << '\n';
	}
	std::cout << '\n';
	
	for (const auto &sym: myFile.getSym()) {
		if (sym.section >= 0xff00) continue;
		std::cout << "Sym '" << sym.name << "':\n";
		std::cout << "  Value: 0x" << std::hex << sym.value << '\n';
		std::cout << "  Size:  " << std::dec << sym.size << '\n';
		std::cout << "  Info:  0x" << std::hex <<  (int) sym.info << '\n';
		std::cout << "  Other: 0x" << (int) sym.other << '\n';
		if (sym.section) {
			std::cout << "  Sect:  '" << myFile.getSect()[sym.section].name << "'\n";
		} else {
			std::cout << "  Sect:  N/A\n";
		}
		std::cout << std::dec << '\n';
	}
	std::cout << '\n';
	
	std::cout << "Attempting to load!\n";
	auto loaded = myFile.load([](size_t vaddr, size_t len, size_t align) -> size_t {
		size_t mem = (size_t) malloc(len + align-1);
		if (mem % align) {
			return mem + align - (mem % align);
		} else {
			return mem;
		}
	});
	
	if (loaded) {
		std::cout << "Loaded:\n";
		std::cout << "  Req. vaddr: " << std::hex << loaded.vaddr_req << '\n';
		std::cout << "  Real vaddr: " << loaded.vaddr_real << '\n';
		std::cout << "  Offs vaddr: " << loaded.vaddr_offset() << '\n';
	} else {
		std::cout << "Load failure.\n";
		while (1) vTaskDelay(1000);
	}
	
	auto sym = myFile.findSym("mydata");
	if (sym) {
		const char *mem = (const char *) (sym->value + loaded.vaddr_offset());
		std::cout << "Sym 'mydata':\n";
		std::cout << "  Addr:  0x" << std::hex << (size_t) mem << '\n';
		std::cout << "  Value: " << mem << '\n';
	} else {
		std::cout << "Symbol 'mydata' not found!\n";
	}
	
	using MessagePrinter = void*(*)(const char *);
	using EF = int(*)(MessagePrinter);
	
	elf::SymMap map;
	map["wrapper"] = (size_t) &callback;
	bool linked = elf::relocate(myFile, loaded, map);
	if (!linked) {
		std::cout << "Link error!\n";
		while (1) vTaskDelay(1000);
	}
	
	std::cout << "Attempting to execute!\n";
	EF entry = (EF) (myFile.getHeader().entry + loaded.vaddr_offset());
	std::cout << "EV = 0x" << std::hex << (size_t) entry << '\n';
	int res = entry((MessagePrinter) +[](const char *msg) -> void* {
		std::cout << "MessagePrinter: " << msg << '\n';
		return nullptr;
	});
	std::cout << "Exit code: 0x" << res << '\n';
	
	while (1) vTaskDelay(1000);
}
*/
