/*
	MIT License

	Copyright (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "elfloader.hpp"
#include "elfloader_int.hpp"


namespace elf {

#ifdef ELFLOADER_MACHINE
// Machine type to check against.
uint16_t machineType = ELFLOADER_MACHINE;
#else
// Machine type to check against.
uint16_t machineType = 0;
#endif



// Load headers and validity-check ELF file.
// File descriptor not closed by this class.
ELFFile::ELFFile(FILE *fd): fd(fd) {
	valid = readHeader();
}

// Read header information and check validity.
// Returns success status.
bool ELFFile::readHeader() {
	// Check magic.
	SEEK(0);
	EXPECT(4, magic);
	
	// Dump data into the STRUCT.
	SEEK(0);
	READ(&header, sizeof(header));
	
	// Check EI_CLASS.
	#ifdef ELFLOADER_ELF_IS_ELF32
	if (header.wordSize == 2) {
		LOGE("ELF file is 64-bit, host is 32-bit");
		return false;
	} else if (header.wordSize != 1) {
		LOGE("ELF file invalid (e_ident[EI_CLASS])");
		return false;
	}
	#else
	if (header.wordSize == 1) {
		LOGE("ELF file is 32-bit, host is 64-bit");
		return false;
	} else if (header.wordSize != 2) {
		LOGE("ELF file invalid (e_ident[EI_CLASS])");
		return false;
	}
	#endif
	
	// Determine host endianness.
	bool hostLE;
	{
		int dummy = 0x0001;
		hostLE = *(bool *) &dummy;
	}
	
	// Check EI_DATA.
	if (header.endianness == 1 && !hostLE) {
		LOGE("ELF file is little-endian, host is little-endian");
		return false;
	} else if (header.endianness == 2 && hostLE) {
		LOGE("ELF file is big-endian, host is little-endian");
		return false;
	} else if (header.endianness != 1 && header.endianness != 2) {
		LOGE("ELF file invalid (e_ident[EI_DATA])");
		return false;
	}
	
	// Check machine type.
	if (machineType && machineType != header.machine) {
		LOGE("ELF file has machine type 0x%04x, host has machine type 0x%04x", header.machine, machineType);
		return false;
	}
	
	// Check miscellaneous constants.
	if (header.size != sizeof(header)) {
		LOGE("ELF file invalid (e_ehsize)");
		return false;
	} else if (header.version != 1) {
		LOGE("ELF file invalid (e_ident[EI_VERSION])");
		return false;
	} else if (header.version2 != 1) {
		LOGE("ELF file invalid (e_version)");
		return false;
	}
	
	// At this point, it can be considered valid.
	return true;
}

// If valid, load section headers.
// Returns success status.
bool ELFFile::readSect() {
	if (!valid) return false;
	
	// Start reading some data.
	for (auto i = 0; i < header.shEntNum; i++) {
		// Read raw section header data.
		SectInfo sh;
		SEEK(header.shOffset + i * header.shEntSize);
		READ(&sh, sizeof(SectHeader));
		
		sectHeaders.push_back(std::move(sh));
	}
	
	// Enforce presence of the name table.
	if (!header.shStrIndex || header.shStrIndex >= sectHeaders.size()) {
		LOGE("ELF file invalid (e_shstrndx)");
		return false;
	}
	
	// Read raw name strings.
	auto &nameSect = sectHeaders[header.shStrIndex];
	std::vector<char> cache;
	cache.reserve(nameSect.file_size);
	SEEK(nameSect.offset);
	READ(cache.data(), nameSect.file_size);
	
	// Second pass to assign names to sections.
	for (auto &sect: sectHeaders) {
		// Bounds checking.
		if (sect.name_index >= nameSect.file_size) {
			LOGE("ELF file invalid (sh_name)");
			return false;
		}
		
		// Determine length.
		size_t maxLen = nameSect.file_size - sect.name_index - 1;
		size_t len = strnlen(cache.data() + sect.name_index, maxLen);
		
		// Copy the string from the cache.
		sect.name.assign(cache.data() + sect.name_index, len);
	}
	
	return true;
}

// If valid, load program headers.
// Returns success status.
bool ELFFile::readProg() {
	if (!valid) return false;
	
	// Start reading some data.
	for (size_t i = 0; i < header.phEntNum; i++) {
		// Read raw program header data.
		ProgInfo ph;
		SEEK(header.phOffset + i * header.phEntSize);
		READ(&ph, sizeof(ProgHeader));
		
		progHeaders.push_back(ph);
	}
	
	return true;
}

// If valid, read non-alocable symbols.
// Returns success status.
bool ELFFile::readSym() {
	if (!valid) return false;
	
	// Find `.symtab` section.
	const auto *symtab = findSect(".symtab");
	if (!symtab) return true;
	
	// Validate `.symtab` symtabion.
	if (symtab->type != (int) SHT::SYMTAB) {
		LOGE("ELF file invalid (`.symtab`: sh_type = 0x%08x)", (unsigned) symtab->type);
		return false;
	}
	if (!symtab->link || symtab->link >= sectHeaders.size()) {
		LOGE("ELF file invalid (`.symtab`: sh_link = 0x%08x)", (unsigned) symtab->type);
		return false;
	}
	
	// Find `.strtab` section.
	const auto &strtab = sectHeaders[symtab->link];
	
	// Start reading some data.
	for (size_t i = 0; i < symtab->file_size / symtab->entry_size; i++) {
		// Read raw symbol entry data.
		SymInfo sym;
		SEEK(symtab->offset + i * symtab->entry_size);
		READ(&sym, sizeof(SymEntry));
		
		// Bounds check.
		if (sym.section >= sectHeaders.size() && sym.section < 0xff00) {
			LOGE("ELF file invalid (st_shndx = 0x%04x)", sym.section);
			return false;
		}
		
		symbols.push_back(std::move(sym));
	}
	
	// Read raw name strings.
	std::vector<char> cache;
	cache.reserve(strtab.file_size);
	SEEK(strtab.offset);
	READ(cache.data(), strtab.file_size);
	
	// Second pass to assign names to symbols.
	for (auto &sym: symbols) {
		// Bounds checking.
		if (sym.name_index >= strtab.file_size) {
			LOGE("ELF file invalid (st_name = %d)", (int) sym.name_index);
			return false;
		}
		
		// Determine length.
		size_t maxLen = strtab.file_size - sym.name_index - 1;
		size_t len = strnlen(cache.data() + sym.name_index, maxLen);
		
		// Copy the string from the cache.
		sym.name.assign(cache.data() + sym.name_index, len);
	}
	
	return true;
}

// If valid, read alocable symbols.
// Returns success status.
bool ELFFile::readDynSym() {
	if (!valid) return false;
	
	// Find `.symtab` section.
	const auto *symtab = findSect(".dynsym");
	if (!symtab) return true;
	
	// Validate `.symtab` symtabion.
	if (symtab->type != (int) SHT::DYNSYM) {
		LOGE("ELF file invalid (`.dynsym`: sh_type = 0x%08x)", (unsigned) symtab->type);
		return false;
	}
	if (!symtab->link || symtab->link >= sectHeaders.size()) {
		LOGE("ELF file invalid (`.dynsym`: sh_link = 0x%08x)", (unsigned) symtab->type);
		return false;
	}
	
	// Find `.strtab` section.
	const auto &strtab = sectHeaders[symtab->link];
	
	// Start reading some data.
	for (size_t i = 0; i < symtab->file_size / symtab->entry_size; i++) {
		// Read raw symbol entry data.
		SymInfo sym;
		SEEK(symtab->offset + i * symtab->entry_size);
		READ(&sym, sizeof(SymEntry));
		
		// Bounds check.
		if (sym.section >= sectHeaders.size() && sym.section < 0xff00) {
			LOGE("ELF file invalid (st_shndx = 0x%04x)", sym.section);
			return false;
		}
		
		dynSym.push_back(std::move(sym));
	}
	
	// Read raw name strings.
	std::vector<char> cache;
	cache.reserve(strtab.file_size);
	SEEK(strtab.offset);
	READ(cache.data(), strtab.file_size);
	
	// Second pass to assign names to symbols.
	for (auto &sym: dynSym) {
		// Bounds checking.
		if (sym.name_index >= strtab.file_size) {
			LOGE("ELF file invalid (st_name = %d)", (int) sym.name_index);
			return false;
		}
		
		// Determine length.
		size_t maxLen = strtab.file_size - sym.name_index - 1;
		size_t len = strnlen(cache.data() + sym.name_index, maxLen);
		
		// Copy the string from the cache.
		sym.name.assign(cache.data() + sym.name_index, len);
	}
	
	return true;
}

// Read all data in the ELF file.
// Returns success status.
bool ELFFile::read() {
	if (!valid) valid = readHeader();
	return valid & readProg() && readSect() && readSym() && readDynSym();
}

// Read data required for loading from the ELF file.
// Returns success status.
bool ELFFile::readDyn() {
	if (!valid) valid = readHeader();
	return valid & readProg() && readSect() && readDynSym();
}


// If valid, load into memory.
// Returns success status.
Program ELFFile::load(Allocator alloc) {
	if (!valid || !readProg()) return {};
	Program out;
	
	// Determine size and address.
	Addr addrMin = -1;
	Addr addrMax = 0;
	for (const auto &prog: progHeaders) {
		Addr al = prog.vaddr;
		Addr ah = prog.vaddr + prog.mem_size;
		if (al < addrMin) addrMin = al;
		if (ah > addrMax) addrMax = ah;
	}
	
	// TODO: Determine alignment.
	Addr align = 32;
	
	// Get memory.
	out.vaddr_req = addrMin;
	auto allocation = alloc(addrMin, addrMax - addrMin, align);
	out.vaddr_real = allocation.first;
	out.memory = (void *) out.vaddr_real;
	out.memory_cookie = (void *) allocation.second;
	out.size = addrMax - addrMin;
	size_t offs = out.vaddr_real - addrMin;
	if (!out) {
		LOGE("Unable to allocate %zu bytes for loading", out.size);
		return {};
	}
	out.entry = (void *) (header.entry + out.vaddr_offset());
	
	// Copy datas.
	for (const auto &prog: progHeaders) {
		// Skip non-resident segments.
		if (prog.type != (int) PHT::LOAD) continue;
		
		fseek(fd, prog.offset, SEEK_SET);
		size_t addr = prog.vaddr + offs;
		fread((void *) addr, 1, prog.file_size, fd);
		memset((void *) (addr + prog.file_size), 0, prog.mem_size - prog.file_size);
		
		char r = prog.flags & 0x4 ? 'r' : '-';
		char w = prog.flags & 0x2 ? 'w' : '-';
		char x = prog.flags & 0x1 ? 'x' : '-';
		LOGD("Prog 0x%x bytes at 0x%x %c%c%c", (int) prog.file_size, (int) (prog.offset + offs), r,w,x);
	}
	
	return out;
}


// Find section by name.
const SectInfo *ELFFile::findSect(const std::string &name) const {
	for (const auto &sect: sectHeaders) {
		if (sect.name == name) return &sect;
	}
	return nullptr;
}

// Find symbol by name.
const SymInfo *ELFFile::findSym(const std::string &name) const {
	for (const auto &sym: symbols) {
		if (sym.name == name) return &sym;
	}
	return nullptr;
}

} // namespace elf
