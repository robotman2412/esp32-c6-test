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

#include "abi.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include <esp_system.h>

#include <malloc.h>
#include <stdio.h>
#include <string.h>

extern "C" const char abiCallTemplate[];
extern "C" const char abiCallTemplateEnd[];

namespace abi {

static elf::SymMap cache;
static std::vector<kernel::fptr_t> abiTable;
static std::unordered_map<int, Context> contextMap;
static int nextPID = 1;



// Map of ABI contexts.
const std::unordered_map<int, Context> &getContexts() {
	return contextMap;
}

// Create a new numbered ABI context.
// You should set the PID from the kernel context to equal the context's PID.
Context &newContext() {
	int pid = nextPID++;
	auto &ref = contextMap[pid];
	ref.pid = pid;
	return ref;
}

// Get a context given a PID.
Context *getContext(int pid) {
	auto iter = contextMap.find(pid);
	if (iter == contextMap.end()) {
		return nullptr;
	} else {
		return &iter->second;
	}
}

// Destroy an ABI context.
bool deleteContext(Context &context) {
	return contextMap.erase(context.getPID());
}

// Destroy an ABI context.
bool deleteContext(int pid) {
	return contextMap.erase(pid);
}



Context::Context() {}

Context::~Context() {
	for (auto mem: actual) {
		free((void *) mem.base);
	}
}

// Merge touching entries of available ranges.
void Context::mergeAvailable() {
	// Sort available ranges by base address.
	// std::sort(available.begin(), available.end(), [](MemRange a, MemRange b) {
	// 	return a.base < b.base;
	// });
	
	for (size_t i = 0; i < available.size() - 1;) {
		MemRange &r0 = available[i].promise;
		MemRange &r1 = available[i+1].promise;
		
		// Test whether ranges touch.
		if (r0.base + r0.length == r1.base) {
			// They touch; merge ranges an check again.
			r0.length += r1.length;
			available.erase(available.begin() + i + 1);
		} else {
			// They don't touch; check next ranges.
			i++;
		}
	}
}

// Insert a range into available such that available remains sorted.
void Context::insertAvailable(MemMapped add) {
	#if 0
	// Binary search the position to insert in to.
	ssize_t begin = 0, end = available.size();
	
	while (begin < end) {
		// Compare against midpoint.
		ssize_t midpoint = (begin + end) / 2;
		if (add < available[midpoint]) {
			// Go lower.
			end = midpoint;
		} else {
			// Go higher.
			begin = midpoint + 1;
		}
	}
	
	// Final insertion.
	available.insert(available.begin() + begin, add);
	#else
	// Linear search the position to insert in to.
	size_t i;
	for (i = 0; i < available.size() && add > available[i]; i++);
	
	// Final insertion.
	available.insert(available.begin() + i, add);
	#endif
}

// Map a new range of a minimum size.
// Returns pointer on success, 0 otherwise.
// The minimum provided alignment shall be `sizeof(size_t)`.
size_t Context::map(size_t min_length, bool allow_write, bool allow_exec, size_t min_align) {
	// Enforce alignment is a power of two.
	if (min_align & (min_align - 1) || !min_length) {
		return 0;
	}
	
	// Set minimum alignment.
	if (min_align <= sizeof(size_t)) {
		min_align = sizeof(size_t);
	}
	if (min_length % min_align) {
		min_length += min_align - min_length % min_align;
	}
	
	size_t base;
	MemRange mem;
	if (min_align <= sizeof(size_t)) {
		// No need for additional alignment of output memory.
		mem  = allocator(min_length, allow_write, allow_exec);
		if (!mem.base) return 0;
		base = mem.base;
		
	} else {
		// Possible need for additional alignment of output memory.
		mem  = allocator(min_length + min_align, allow_write, allow_exec);
		if (!mem.base) return 0;
		if (mem.base % min_align) {
			base = mem.base + min_align - mem.base % min_align;
		} else {
			base = mem.base;
		}
	}
	
	// Actual allocation.
	size_t actual_i = actual.size();
	actual.push_back(mem);
	
	// Unused space before.
	if (base > mem.base) {
		insertAvailable({{mem.base, base-mem.base}, actual_i});
	}
	
	// Used space.
	mapped.push_back({{base, min_length}, actual_i});
	
	// Unused space after.
	if (base + min_length < mem.base + mem.length) {
		insertAvailable({{mem.base, mem.length-base-min_length}, actual_i});
	}
	
	return base;
}

// Actual implementation of unmap.
// Index is into mapped list.
void Context::unmapIdx(size_t index) {
	// Move a block from mapped to available.
	auto range = mapped[index];
	mapped.erase(mapped.begin() + index);
	insertAvailable(range);
	
	// Check for the possibility to release resources.
	mergeAvailable();
	for (size_t i = 0; i < available.size();) {
		size_t j   = available[i].actual;
		auto sub   = available[i].promise;
		auto super = actual[j];
		
		// If ranges match then the memory can be released.
		if (sub == super) {
			// Remove memory ranges.
			actual.erase(actual.begin() + available[i].actual);
			available.erase(available.begin() + i);
			
			// Correct indices.
			for (size_t x = 0; x < available.size(); x++) {
				if (available[x].actual >= j) {
					available[x].actual --;
				}
			}
			for (size_t x = 0; x < mapped.size(); x++) {
				if (mapped[x].actual >= j) {
					mapped[x].actual --;
				}
			}
			
			// Release memory.
			deallocator(super);
			
		} else {
			// Check next range.
			i++;
		}
	}
}

// Unmap a range of memory.
bool Context::unmap(size_t base) {
	// Locate matching range.
	for (size_t i = 0; i < mapped.size(); i++) {
		if (base == mapped[i].promise.base) {
			// Match found.
			unmapIdx(i);
			return true;
		}
	}
	// Match not found.
	return false;
}



// An overridable allocator used for Context.
// The minimum provided alignment shall be `sizeof(size_t)`.
MemRange allocator(size_t min_length, bool allow_write, bool allow_exec) __attribute__((weak));
MemRange allocator(size_t min_length, bool allow_write, bool allow_exec) {
	void *mem = malloc(min_length);
	return { (size_t) mem, min_length };
}

// An overridable allocator used for Context.
void deallocator(MemRange range) __attribute__((weak));
void deallocator(MemRange range) {
	free((void *) range.base);
}



// Generates an ABI call wrapper function.
static void *writeABIWrapper(int abiIndex) {
	// Allocate an memories.
	size_t len = abiCallTemplateEnd - abiCallTemplate;
	size_t cap = len + sizeof(int);
	void  *mem = malloc(cap);
	if (!mem) return nullptr;
	
	// Copy in the generic.
	memcpy(mem, (const void *) abiCallTemplate, len);
	// Copy in the INDEX WORD.
	uint32_t *indexPtr = (uint32_t *) (len + (size_t) mem);
	*indexPtr = abiIndex;
	
	// Done!
	return mem;
}

static void initCache() {
	exportSymbolsUnwrapped(cache);
	for (auto &entry: cache) {
		auto fptr = entry.second;
		entry.second = (size_t) writeABIWrapper(abiTable.size());
		abiTable.push_back((kernel::fptr_t) fptr);
	}
}

// Called when an app aborts.
static void appAborted() {
	printf("App aborted!\n");
	abort();
}

// Exports ABI symbols into `map`.
void exportSymbolsUnwrapped(elf::SymMap &map) {
	// From system.h:
	map["yield"]    = (size_t) +[]{
		kernel::ctx_t *ctx = kernel::getCtx();
		vPortYield();
		if (ctx) kernel::setCtx(ctx);
	};
	map["delay_ms"] = (size_t) +[](int64_t millis){
		kernel::ctx_t *ctx = kernel::getCtx();
		vTaskDelay(pdMS_TO_TICKS(millis));
		if (ctx) kernel::setCtx(ctx);
	};
	map["delay_us"] = (size_t) +[](int64_t micros){
		kernel::ctx_t *ctx = kernel::getCtx();
		int64_t millis = micros / 1000;
		if (millis >= 4) {
			micros -= millis * 1000;
			int64_t t1 = esp_timer_get_time();
			vTaskDelay(pdMS_TO_TICKS(millis));
			int64_t t2 = esp_timer_get_time();
			int64_t took_us = t2 - t1;
			micros -= took_us;
		}
		esp_rom_delay_us(micros);
		if (ctx) kernel::setCtx(ctx);
	};
	map["sched_yield"] = map["yield"];
	map["usleep"]      = map["delay_us"];
	
	// From malloc.h:
	map["malloc"]  = (size_t) &malloc;
	map["free"]    = (size_t) &free;
	map["calloc"]  = (size_t) &calloc;
	map["realloc"] = (size_t) &realloc;
	
	// From stdio.h:
	map["__get_stdin"]  = (size_t) +[]{ return stdin; };
	map["__get_stdout"] = (size_t) +[]{ return stdout; };
	map["__get_stderr"] = (size_t) +[]{ return stderr; };
	map["__printf_chk"] = (size_t) &printf;
	
	map["remove"]    = (size_t) &remove;
	map["rename"]    = (size_t) &rename;
	map["tmpfile"]   = (size_t) &tmpfile;
	map["tmpnam"]    = (size_t) &tmpnam;
	map["fclose"]    = (size_t) &fclose;
	map["fflush"]    = (size_t) &fflush;
	map["fopen"]     = (size_t) &fopen;
	map["freopen"]   = (size_t) &freopen;
	map["vfprintf"]  = (size_t) &vfprintf;
	map["fprintf"]   = (size_t) &fprintf;
	map["vprintf"]   = (size_t) &vprintf;
	map["printf"]    = (size_t) &printf;
	map["vasprintf"] = (size_t) &vasprintf;
	map["asprintf"]  = (size_t) &asprintf;
	map["vsnprintf"] = (size_t) &vsnprintf;
	map["vsprintf"]  = (size_t) &vsprintf;
	map["snprintf"]  = (size_t) &snprintf;
	map["sprintf"]   = (size_t) &sprintf;
	map["vfscanf"]   = (size_t) &vfscanf;
	map["fscanf"]    = (size_t) &fscanf;
	map["vscanf"]    = (size_t) &vscanf;
	map["scanf"]     = (size_t) &scanf;
	map["vsscanf"]   = (size_t) &vsscanf;
	map["sscanf"]    = (size_t) &sscanf;
	map["fgets"]     = (size_t) &fgets;
	map["fgetc"]     = (size_t) &fgetc;
	map["getchar"]   = (size_t) &getchar;
	map["fread"]     = (size_t) &fread;
	map["ungetc"]    = (size_t) &ungetc;
	map["fputs"]     = (size_t) &fputs;
	map["puts"]      = (size_t) &puts;
	map["fputc"]     = (size_t) &fputc;
	map["putchar"]   = (size_t) &putchar;
	map["fwrite"]    = (size_t) &fwrite;
	map["fgetpos"]   = (size_t) &fgetpos;
	map["fsetpos"]   = (size_t) &fsetpos;
	map["fseek"]     = (size_t) &fseek;
	map["ftell"]     = (size_t) &ftell;
	map["rewind"]    = (size_t) &rewind;
	map["setbuf"]    = (size_t) &setbuf;
	map["setvbuf"]   = (size_t) &setvbuf;
	map["clearerr"]  = (size_t) &clearerr;
	map["feof"]      = (size_t) &feof;
	map["ferror"]    = (size_t) &ferror;
	map["perror"]    = (size_t) &perror;
	
	// From stdlib.h:
	map["atof"]    = (size_t) &atof;
	map["atoi"]    = (size_t) &atoi;
	map["atol"]    = (size_t) &atol;
	map["strtod"]  = (size_t) &strtod;
	map["strtol"]  = (size_t) &strtol;
	map["strtoul"] = (size_t) &strtoul;
	map["rand"]    = (size_t) &rand;
	map["srand"]   = (size_t) &srand;
	map["abort"]   = (size_t) &appAborted;
	map["atexit"]  = (size_t) &atexit;
	map["exit"]    = (size_t) &exit;
	// `getenv` is up to the program itself to implement.
	
	// From string.h:
	map["memchr"]     = (size_t) &memchr;
	map["memrchr"]    = (size_t) &memrchr;
	map["memcmp"]     = (size_t) &memcmp;
	map["memccpy"]    = (size_t) &memccpy;
	map["memcpy"]     = (size_t) &memcpy;
	map["memmove"]    = (size_t) &memmove;
	map["memset"]     = (size_t) &memset;
	map["strchr"]     = (size_t) &strchr;
	map["strrchr"]    = (size_t) &strrchr;
	map["strcmp"]     = (size_t) &strcmp;
	map["strncmp"]    = (size_t) &strncmp;
	map["strcat"]     = (size_t) &strcat;
	map["strncat"]    = (size_t) &strncat;
	map["strspn"]     = (size_t) &strspn;
	map["strcspn"]    = (size_t) &strcspn;
	map["strstr"]     = (size_t) &strstr;
	map["strcasestr"] = (size_t) &strcasestr;
	map["strlen"]     = (size_t) &strlen;
	map["strnlen"]    = (size_t) &strnlen;
	map["strerror"]   = (size_t) &strerror;
}



// Get the ABI TABLE.
kernel::fptr_t *getAbiTable() {
	if (!cache.size()) initCache();
	return abiTable.data();
}

// Get the size of the ABI TABLE.
size_t getAbiTableSize() {
	if (!cache.size()) initCache();
	return abiTable.size();
}

// Exports ABI symbols into `map`.
void exportSymbols(elf::SymMap &map) {
	if (!cache.size()) initCache();
	map.merge(cache);
}

} // namespace abi
