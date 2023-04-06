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

#pragma once

#include "elfloader.hpp"

namespace elf {

// Map storing known symbol values.
using SymMap = std::map<std::string, size_t>;

// Reads an ADDEND for a relocation.
Addr getAddend(const ELFFile &ctx, uint32_t relType, uint8_t *ptr);

// Apply a single relocation.
bool applyRelocation(const ELFFile &ctx, const Program &program, uint32_t relType, Addr symVal, Addr addend, uint8_t *ptr);

// Apply all relocations for the loaded program.
bool relocate(const ELFFile &ctx, const Program &program, const SymMap &map);

// Extract symbols from a loaded program into the map.
bool exportSymbols(const ELFFile &ctx, const Program &program, SymMap &map);

} // namespace elf
