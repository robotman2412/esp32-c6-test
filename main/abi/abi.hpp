/*
	MIT License

	Copyright    (c) 2023 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files    (the "Software"), to deal
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

#include <elfloader.hpp>
#include <relocation.hpp>
#include <kernel.hpp>
#include <vector>
#include <unordered_map>

namespace abi {

// Represents a range of memory.
struct MemRange {
	size_t base, length;
	
	// Determine whether an address falls within this range.
	constexpr bool contains(size_t addr) const {
		return addr >= base && addr < base + length;
	}
	
	// Overload comparison operator.
	bool operator<(const MemRange &other) const { return base < other.base; }
	// Overload comparison operator.
	bool operator>(const MemRange &other) const { return base > other.base; }
	// Overload comparison operator.
	bool operator<=(const MemRange &other) const { return base <= other.base; }
	// Overload comparison operator.
	bool operator>=(const MemRange &other) const { return base >= other.base; }
	// Overload comparison operator.
	bool operator!=(const MemRange &other) const { return base != other.base; }
	// Overload comparison operator.
	bool operator==(const MemRange &other) const { return base == other.base; }
};

// Represents a mapped range of memory.
struct MemMapped {
	// Promised memory range.
	MemRange promise;
	// Index of actual memory range (may be larger).
	size_t actual;
	
	// Overload comparison operator.
	bool operator<(const MemMapped &other) const { return promise.base < other.promise.base; }
	// Overload comparison operator.
	bool operator>(const MemMapped &other) const { return promise.base > other.promise.base; }
	// Overload comparison operator.
	bool operator<=(const MemMapped &other) const { return promise.base <= other.promise.base; }
	// Overload comparison operator.
	bool operator>=(const MemMapped &other) const { return promise.base >= other.promise.base; }
	// Overload comparison operator.
	bool operator!=(const MemMapped &other) const { return promise.base != other.promise.base; }
	// Overload comparison operator.
	bool operator==(const MemMapped &other) const { return promise.base == other.promise.base; }
};

// An ABI context.
class Context;
class Context {
	protected:
		// Actual allocated blocks of memory.
		std::vector<MemRange> actual;
		// List of dynamically allocated memory ranges.
		std::vector<MemMapped> mapped;
		// List of available ranges of memory.
		std::vector<MemMapped> available;
		
		// Process ID.
		int pid;
		
		// Merge touching entries of available ranges.
		void mergeAvailable();
		// Insert a range into available such that available remains sorted.
		void insertAvailable(MemMapped mapped);
		// Actual implementation of unmap.
		// Index is into mapped list.
		void unmapIdx(size_t index);
		
		friend Context &newContext();
		friend void deleteContext();
		
	public:
		// Create a new context (but you probably shouldn't be calling this).
		Context();
		// Delete a context (again, you probably shouldn't be calling this).
		~Context();
		
		// Context is neither copyable nor movable.
		Context(const Context&) = delete;
		Context &operator=(const Context&) = delete;
		Context(Context&&) = delete;
		Context &operator=(Context&&) = delete;
		
		// Map a new range of a minimum size.
		// Returns pointer on success, 0 otherwise.
		// The minimum provided alignment shall be `sizeof(size_t)`.
		size_t map(size_t min_length, bool allow_write = true, bool allow_exec = false, size_t min_align = sizeof(size_t));
		// Unmap a range of memory.
		// Returns whether base was the base address of a valid range.
		bool unmap(size_t base);
		
		// Get process ID.
		int getPID() const { return pid; }
};

// An overridable allocator used for Context.
// The minimum provided alignment shall be `sizeof(size_t)`.
extern MemRange allocator(size_t min_length, bool allow_write, bool allow_exec);
// An overridable allocator used for Context.
extern void deallocator(MemRange);

// Map of ABI contexts.
const std::unordered_map<int, Context> &getContexts();
// Create a new numbered ABI context.
// You should set the PID from the kernel context to equal the context's PID.
Context &newContext();
// Get a context given a PID.
Context *getContext(int pid);
// Destroy an ABI context.
bool deleteContext(Context &context);
// Destroy an ABI context.
bool deleteContext(int pid);

// Get the ABI TABLE.
kernel::fptr_t *getAbiTable();
// Get the size of the ABI TABLE.
size_t getAbiTableSize();

// Exports ABI symbols into `map` (with wrapper).
void exportSymbols(elf::SymMap &map);
// Exports ABI symbols into `map` (no wrapper).
void exportSymbolsUnwrapped(elf::SymMap &map);

}
