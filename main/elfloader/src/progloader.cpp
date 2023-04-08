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

#include "progloader.hpp"
#include "mpu.hpp"
#include <iostream>

namespace loader {

// Forwards directly to malloc.
size_t mallocForward(size_t vaddr, size_t len, size_t align) {
	return (size_t) malloc(len);
}

// Uses malloc to get an ALIGNED chunk of stuf.
size_t mallocAligned(size_t vaddr, size_t len, size_t align) {
	size_t addr = (size_t) malloc(len + align);
	addr += align - addr % align;
	return addr;
}

// Load a library from a file.
// Returns success status.
bool Linkage::loadLibrary(FILE *fd) {
	// Create reading context.
	auto elf = elf::ELFFile(fd);
	
	// Try to read data.
	if (!elf.readDyn()) return false;
	
	// Try to load progbits.
	auto prog = elf.load(mallocAligned);
	if (!prog) return false;
	
	// Try to perform the linkage.
	if (!elf::relocate(elf, prog, symbols)) {
		free(prog.memory);
		return false;
	}
	
	// Export the symbols.
	if (!elf::exportSymbols(elf, prog, symbols)) {
		free(prog.memory);
		return false;
	}
	
	// Add to loaded things list.
	loaded.push_back(prog);
	
	// Apply MPU shenanigans.
	if (!mpu::applyPH(elf, prog)) {
		std::cout << "Applying MPU settings failed, ignoring.\n";
	}
	
	return true;
}

// Load the executable from a file.
// Returns success status.
bool Linkage::loadExecutable(FILE *fd) {
	// Yep this can be easily forwarded.
	if (loadLibrary(fd)) {
		entryFunc = loaded.back().entry;
		return true;
	}
	return false;
}

};
