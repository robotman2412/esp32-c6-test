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

#include "stddef.h"

#ifndef _LCS_REST
# ifdef __cplusplus
#  define _LCS_REST
# else
#  define _LCS_REST restrict
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Allocate at least `__len` bytes of memory.
// If `__len` is 0 or there is not enough memory to fit `__len` bytes, NULL may be returned.
void *malloc(size_t __len);
// Release memory previously acquired using `malloc`, `calloc` or `realloc`.
// If `__mem` is NULL, no operation is performed.
void  free  (void *__mem);
// Allocate at least `__size*__count` bytes of memory.
// If `__size*__count` is 0 or there is not enough memory to fit `__size*__count` bytes, NULL may be returned.
void *calloc(size_t __count, size_t __size);
// Resize memory previously acquired using `malloc`, `calloc` or `realloc`.
// May either move the memory or expand the allocation in place.
// If `__orig` is NULL, this is equal to malloc.
// Returns NULL (but does not release memory!) when there is not enough memory to fit `__len` bytes.
void *realloc(void *__orig, size_t __len);

#ifdef __cplusplus
} // extern "C"
#endif
