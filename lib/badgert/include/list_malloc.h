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

#include <malloc.h>
#include <system.h>

// Get amount of free heap space.
size_t list_get_free_heap_space();
// Get amount of allocated memory.
size_t list_get_used_heap_space();
// Get amount of raw memory usage.
size_t list_get_used_raw();

// Initialise memory allocator.
void list_malloc_init();
// Allocate `size` bytes of memory.
void *list_malloc(size_t size);
// Reallocate `mem` to become `size` bytes.
void *list_realloc(void *mem, size_t size);
// Free `mem`.
void list_free(void *mem);
// Get real size of `mem`.
size_t list_alloc_size(void *mem);

// Print debug info.
void list_alloc_debug();
