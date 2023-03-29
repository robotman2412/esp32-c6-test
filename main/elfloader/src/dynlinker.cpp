
#include "dynlinker.hpp"
#include "elfloader_int.hpp"

namespace elf {

// Whether the symbol name is a magic PLTF name.
bool isPLTFName(const std::string &str) {
	return !strncmp(str.c_str(), "__pltf_", 7);
}


// Link symbols from `map` into `program` dynamically.
bool importLink(const ELFFile &elf, const Program &program, SymMap &map) {
	for (auto &sym: elf.getSym()) {
		// Skip non-PLTF symbols.
		if (!isPLTFName(sym.name)) continue;
		
		// Look it up in the map.
		auto key  = sym.name.substr(7);
		auto iter = map.find(key);
		if (iter == map.end()) {
			LOGE("Undefined symbol '%s'", key.c_str());
			return false;
		}
		
		// Update the pointer in memory.
		size_t addr = sym.value + program.vaddr_offset();
		*(size_t *) addr = iter->second;
	}
	return true;
}

// Export symbols from `program` into `map`.
bool exportLink(const ELFFile &elf, const Program &program, SymMap &map) {
	for (auto &sym: elf.getSym()) {
		// Skip PLTF symbols.
		if (isPLTFName(sym.name)) continue;
		
		// Add to map if not already.
		auto iter = map.find(sym.name);
		if (iter == map.end()) {
			map[sym.name] = sym.value + program.vaddr_offset();
		}
	}
	return true;
}

// Link symbols from `map` into `program` followed by exporting symbols from `program` into `map`.
bool link(const ELFFile &elf, const Program &program, SymMap &map) {
	return importLink(elf, program, map)
		&& exportLink(elf, program, map);
}

} // namespace elf
