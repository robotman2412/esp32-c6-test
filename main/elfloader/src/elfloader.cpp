
#include "elfloader.hpp"

#define ELFLOADER_LINENUMBERS


#ifdef ELFLOADER_LINENUMBERS
#define LOG_LNFMT "%s:%d: "
#define LOG_LNARG , __FILE__, __LINE__
#else
#define LOG_LNFMT ""
#define LOG_LNARG
#endif

#ifdef ESP_PLATFORM
#include <esp_log.h>
static const char *TAG = "elfloader";
#define LOGE(fmt, ...) ESP_LOGE(TAG, LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGI(fmt, ...) ESP_LOGI(TAG, LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGW(fmt, ...) ESP_LOGW(TAG, LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGD(fmt, ...) ESP_LOGD(TAG, LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#else
static void _elf_log_helper(const char *prefix, const char *fmt, ...) {
	fputs(prefix, stdout);
	va_list ls;
	va_start(ls, fmt);
	vprintf(fmt, ls);
	va_end(ls);
	fputs("\033[0m", stdout);
}
#define LOGE(fmt, ...) _elf_log_helper("\033[31mErr  elfloader: ", LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGI(fmt, ...) _elf_log_helper( "\033[0mInfo elfloader: ", LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGW(fmt, ...) _elf_log_helper("\033[33mWarn elfloader: ", LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#define LOGD(fmt, ...) _elf_log_helper("\033[34mDeb  elfloader: ", LOG_LNFMT fmt LOG_LNARG __VA_OPT__(,) __VA_ARGS__)
#endif



// A magic tester for multi boits
static bool _elf_expect(FILE *fd, size_t len, const void *magic) {
	uint8_t tmp[len];
	size_t read = fread(tmp, 1, len, fd);
	if (read != len) return false;
	return !memcmp(tmp, magic, len);
}
#define EXPECT(count, magic) do { errno = 0; if (!_elf_expect(fd, count, magic)) {LOGE("I/O error: %s", strerror(errno)); return false;} } while(0)

// A INTEGER READING DEVICE
static bool _elf_readint(FILE *fd, uint64_t *out, size_t size, bool is_signed, bool is_le) {
	uint64_t tmp = 0;
	
	if (is_le) {
		for (size_t i = 0; i < size; i++) {
			int c = fgetc(fd);
			if (c == EOF) return false;
			tmp |= (uint64_t) (c & 0xff) << (i*8);
		}
	} else {
		for (size_t i = 0; i < size; i++) {
			int c = fgetc(fd);
			if (c == EOF) return false;
			tmp |= c << ((size-i-1)*8);
		}
	}
	
	if (is_signed && (tmp >> (size*8-1))) {
		for (size_t i = size; i < 8; i++) {
			tmp |= 0xffllu << (i*8);
		}
	}
	
	*out = tmp;
	return true;
}
#define READUINT(dest, size) do { errno = 0; uint64_t tmp; if (!_elf_readint(fd, &tmp, size, false, is_little_endian)) {LOGE("I/O error: %s", strerror(errno)); return false;} (dest) = tmp; } while(0)

// SKIPS PADDING.
static bool _elf_skip(FILE *fd, size_t len) {
	long pre = ftell(fd);
	fseek(fd, len, SEEK_CUR);
	return ftell(fd) - pre == len;
}
#define SKIP(size) do { errno = 0; if (!_elf_skip(fd, (size))) {LOGE("I/O error: %s", strerror(errno)); return false;} } while(0)

// SEEKD LOKATON.
static bool _elf_seek(FILE *fd, size_t idx) {
	fseek(fd, idx, SEEK_SET);
	return ftell(fd) == idx;
}
#define SEEK(idx) do { errno = 0; if (!_elf_seek(fd, (idx))) {LOGE("I/O error: %s", strerror(errno)); return false;} } while(0)

// READ BIANIER.
#define READ(ptr, len) do { auto tmplen = (len); errno = 0; if (fread(ptr, 1, tmplen, fd) != tmplen) {LOGE("I/O error: %s", strerror(errno)); return false;} } while(0)


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

// If valid, read symbols.
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
		READ(&sym, sizeof(SymHeader));
		
		// Bounds check.
		if (sym.section >= 0xff00) continue;
		if (sym.section >= sectHeaders.size()) {
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

// Read all data in the ELF file.
// Returns success status.
bool ELFFile::read() {
	if (!valid) valid = readHeader();
	return valid & readProg() && readSect() && readSym();
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
	out.vaddr_real = alloc(addrMin, addrMax - addrMin, align);
	out.memory = (void *) out.vaddr_real;
	out.size = addrMax - addrMin;
	size_t offs = out.vaddr_real - addrMin;
	if (!out) {
		LOGE("Unable to allocate %zu bytes for loading", out.size);
		return {};
	}
	
	// Copy datas.
	for (const auto &prog: progHeaders) {
		fseek(fd, prog.offset, SEEK_SET);
		size_t addr = prog.vaddr + offs;
		fread((void *) addr, 1, prog.file_size, fd);
		memset((void *) (addr + prog.file_size), 0, prog.mem_size - prog.file_size);
	}
	
	return out;
}


// Find section by name.
const SectInfo *ELFFile::findSect(const std::string &name) {
	for (const auto &sect: sectHeaders) {
		if (sect.name == name) return &sect;
	}
	return nullptr;
}

// Find symbol by name.
const SymInfo *ELFFile::findSym(const std::string &name) {
	for (const auto &sym: symbols) {
		if (sym.name == name) return &sym;
	}
	return nullptr;
}

} // namespace elf
