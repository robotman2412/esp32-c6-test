
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <vector>
#include <string>
#include <map>
#include <functional>

#include "elfloader_machine.hpp"



namespace elf {

#if SIZE_MAX > 0xFFFFFFFFLLU
using Addr = uint64_t;
#define ELFLOADER_ELF_IS_ELF64
#else
using Addr = uint32_t;
#define ELFLOADER_ELF_IS_ELF32
#endif

// First four bytes of an ELF file.
static const char magic[4] = { 0x7f, 'E', 'L', 'F' };
// Machine type to check against.
extern uint16_t machineType;



// ELF file type.
enum class ET {
	NONE = 0x00,
	REL  = 0x01,
	EXEC = 0x02,
	DYN  = 0x03,
};

// Section header type.
enum class SHT {
	PROGBITS = 1,
	SYMTAB   = 2,
	STRTAB   = 3,
	RELA     = 4,
	HASH     = 5,
	DYNAMIC  = 6,
	NOTE     = 7,
	NOBITS   = 8,
	REL      = 9,
	SHLIB    = 10,
	DYNSYM   = 11,
};

// Program header type.
enum class PHT {
	UNUSED  = 0x00,
	LOAD    = 0x01,
	DYNAMIC = 0x02,
	INTERP  = 0x03,
	NOTE    = 0x04,
	SHLIB   = 0x05,
	PHDR    = 0x06,
	TLS     = 0x07,
};

// Symbol type.
enum class STT {
	NOTYPE  = 0,
	OBJECT  = 1,
	FUNC    = 2,
	SECTION = 3,
	FILE    = 4,
};


// Common (32-bit and 64-bit) ELF file header information.
struct Header {
	// Magic: 0x7f, 'E', 'L', 'F'.
	char     magic[4];
	// Class: 1 or 2 for 32- or 64-bit respectively.
	uint8_t  wordSize;
	// Endianness: 1 or 2 for little or big respectively.
	uint8_t  endianness;
	// ELF file version.
	uint8_t  version;
	// OS/ABI type.
	uint8_t  osabi;
	// More OS/ABI data.
	uint8_t  ident;
	// Padding bytes.
	char _padding0[7];
	// ELF file type.
	uint16_t type;
	// Machine type.
	uint16_t machine;
	// A copy of the version.
	uint32_t version2;
	// Program entry point.
	Addr     entry;
	// File offset of program header table.
	Addr     phOffset;
	// File offset of section header table.
	Addr     shOffset;
	// Target-dependent flags.
	uint32_t flags;
	// Size of this header, 64 bytes for 64-bit or 52 bytes for 32-bit.
	uint16_t size;
	// Size of a program header table entry.
	uint16_t phEntSize;
	// Number of entries in the program header table.
	uint16_t phEntNum;
	// Size of a program header table entry.
	uint16_t shEntSize;
	// Number of entries in the program header table.
	uint16_t shEntNum;
	// Index of the section containing the section name table.
	uint16_t shStrIndex;
};
static_assert(sizeof(Header) == 0x34 || sizeof(Header) == 0x40, "elf::Header must be either 0x34 or 0x40 bytes in size.");

// Section header information.
struct SectHeader {
	// Index in the name table.
	uint32_t    name_index;
	// Type of section.
	uint32_t    type;
	// Flags bitmap.
	Addr        flags;
	// Virtual address for loadable sections.
	Addr        vaddr;
	// Offset in the file image of section data.
	Addr        offset;
	// Size in the file image.
	Addr        file_size;
	// A related section, if any.
	uint32_t    link;
	// Type dependent additional info.
	uint32_t    info;
	// Alignment, must be an integer power of two.
	Addr        alignment;
	// The size, in bytes, of an entry, for sections with fixed-size entries.
	Addr        entry_size;
};
static_assert(sizeof(SectHeader) == 0x28 || sizeof(SectHeader) == 0x40, "elf::SectHeader must either be 0x28 or 0x40 bytes in size.");

// Program header information.
struct ProgHeader {
	// Type of the segment.
	uint32_t type;
#ifdef ELFLOADER_ELF_IS_ELF64
	// Flags bitfield.
	uint32_t flags;
#endif
	// Offset in the file image.
	Addr   offset;
	// Virtual address of segment.
	Addr   vaddr;
	// Physical address, if any.
	Addr   paddr;
	// Size in the file image in bytes.
	Addr   file_size;
	// Size in memory.
	Addr   mem_size;
#ifdef ELFLOADER_ELF_IS_ELF32
	// Flags bitfield.
	uint32_t flags;
#endif
	// Alignment, must be an integer power of two.
	Addr   alignment;
};
static_assert(sizeof(ProgHeader) == 0x20 || sizeof(ProgHeader) == 0x38, "elf::ProgHeader must be either 0x20 or 0x38 bytes in size.");

// Symbol header information.
struct SymHeader {
	// Index in the name table.
	uint32_t name_index;
#ifdef ELFLOADER_ELF_IS_ELF32
	// Symbol value, if any.
	Addr     value;
	// Symbol size in bytes.
	uint32_t size;
#endif
	// Type and attributes.
	uint8_t  info;
	// Symbol visibility.
	uint8_t  other;
	// Section index of this symbol. 0 means the symbol is undefined.
	uint16_t section;
#ifdef ELFLOADER_ELF_IS_ELF64
	// Symbol value, if any.
	Addr     value;
	// Symbol size in bytes.
	uint64_t size;
#endif
};
static_assert(sizeof(SymHeader) == 0x10 || sizeof(SymHeader) == 0x18, "elf::SymHeader must be either 0x10 or 0x18 bytes in size.");



// Section header but with a name.
struct SectInfo: public SectHeader {
	std::string name;
};

using ProgInfo = ProgHeader;

// Symbol header but with a name.
struct SymInfo: public SymHeader {
	std::string name;
	
	bool isFunction() const {
		return (info & 0x0f) == (int) STT::FUNC;
	}
};



// Loaded instance of an ELF file.
struct Program {
	// Requested vaddr.
	Addr vaddr_req;
	// Actual vaddr.
	Addr vaddr_real;
	// Offset (vaddr_real - vaddr_req).
	Addr vaddr_offset() const { return vaddr_real - vaddr_req; }
	
	// Allocated memory.
	void *memory;
	// Size of allocated memory.
	size_t size;
	
	// By default, zero all fields.
	Program():
		vaddr_req(0), vaddr_real(0),
		memory(nullptr), size(0) {}
	
	// True if `memory` is nonnull.
	operator bool() const { return !!memory; }
};



// ELF file reader and loader.
class ELFFile {
	public:
		// Some callback that allocates memory for program loading.
		// Returns pointer to allocated memory on success, zero on failure.
		using Allocator = std::function<size_t(size_t vaddr, size_t len, size_t align)>;
		
		// File descriptor to use for loading.
		// Not closed by this class.
		FILE *fd;
		
	protected:
		// This is a valid ELF file for this machine.
		bool valid;
		
		// Header information.
		Header header;
		// Program headers.
		std::vector<ProgInfo> progHeaders;
		// Section headers.
		std::vector<SectInfo> sectHeaders;
		// Symbols.
		std::vector<SymInfo> symbols;
		
	public:
		// Empty, invalid ELF file.
		ELFFile(): valid(false) {}
		// Load headers and validity-check ELF file.
		// File descriptor not closed by this class.
		ELFFile(FILE *fd);
		
		// Read header information and check validity.
		// Returns success status.
		bool readHeader();
		// If valid, load section headers.
		// Returns success status.
		bool readSect();
		// If valid, load program headers.
		// Returns success status.
		bool readProg();
		// If valid, read symbols.
		// Returns success status.
		bool readSym();
		// Read all data in the ELF file.
		// Returns success status.
		bool read();
		
		// If valid, load into memory.
		Program load(Allocator alloc);
		
		// Is this a VALID?
		bool isValid() const { return valid; }
		// Get read-only copy of header.
		const auto &getHeader() const { return header; }
		// Get read-only copy of section headers.
		const auto &getSect() const { return sectHeaders; }
		// Get read-only copy of program headers.
		const auto &getProg() const { return progHeaders; }
		// Get read-only copy of symbols.
		const auto &getSym() const { return symbols; }
		
		// Find section by name.
		const SectInfo *findSect(const std::string &name) const;
		// Find symbol by name.
		const SymInfo *findSym(const std::string &name)const;
};

} // namespace elf
