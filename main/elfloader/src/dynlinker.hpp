
#pragma once

#include "elfloader.hpp"

namespace elf {

// Map storing known symbol values.
using SymMap = std::map<std::string, size_t>;

// Whether the symbol name is a magic PLTF name.
bool isPLTFName(const std::string &str);

// Link symbols from `map` into `program` dynamically.
bool importLink(const ELFFile &elf, const Program &program, SymMap &map);
// Export symbols from `program` into `map`.
bool exportLink(const ELFFile &elf, const Program &program, SymMap &map);
// Link symbols from `map` into `program` followed by exporting symbols from `program` into `map`.
bool link(const ELFFile &elf, const Program &program, SymMap &map);

} // namespace elf
