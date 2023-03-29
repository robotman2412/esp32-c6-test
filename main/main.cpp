
#include <iostream>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdio.h>
#include <elfloader.hpp>

extern const char elf_start[] asm("_binary_main_o_start");
extern const char elf_end[] asm("_binary_main_o_end");

extern "C" void app_main() {
    std::cout << "LOLOLOL time!\n";
	
	FILE *fd = fmemopen((void *) elf_start, elf_end - elf_start, "r");
	elf::ELFFile myFile(fd);
	
	std::cout << "Header: " << myFile.isValid() << '\n';
	std::cout << "Sect:   " << myFile.readSect() << '\n';
	std::cout << "Prog:   " << myFile.readProg() << '\n';
	std::cout << "Sym:    " << myFile.readSym() << '\n';
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
		std::cout << "Sym '" << sym.name << "':\n";
		std::cout << "  Value: 0x" << std::hex << sym.value << '\n';
		std::cout << "  Size:  " << std::dec << sym.size << '\n';
		std::cout << "  Info:  0x" << std::hex <<  (int) sym.info << '\n';
		std::cout << "  Other: 0x" << (int) sym.other << '\n';
		// if (sym.section) {
		// 	std::cout << "  Sect:  '" << myFile.getSect()[sym.section].name << "'\n";
		// } else {
		// 	std::cout << "  Sect:  N/A\n";
		// }
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
	
	using PLTF = void*(*)(const char *);
	using EF = int(*)(PLTF);
	
	std::cout << "Attempting to execute!\n";
	EF entry = (EF) (myFile.getHeader().entry + loaded.vaddr_offset());
	std::cout << "EV = 0x" << std::hex << (size_t) entry << '\n';
	int res = entry(+[](const char *msg) -> void* {
		std::cout << "PLTF: " << msg << '\n';
		return nullptr;
	});
	std::cout << "Exit code: 0x" << res << '\n';
	
	while (1) vTaskDelay(1000);
}
