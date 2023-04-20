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

#include "malloc.h"

#ifdef __cplusplus
extern "C" {
#endif

// Exit without running any of the exit handlers.
static inline void _exit(int ec) __attribute__((naked)) __attribute__((noreturn));
static inline void _exit(int ec) {
	asm volatile ("mv a1, a0");
	asm volatile ("li a0, 512");
	asm volatile ("ecall");
}

// Convert a c-string `__src` to a double.
extern double atof(const char *__src)
	__attribute__((pure)) __attribute__((__nonnull__(1)));
// Convert a c-string `__src` to an int.
extern int atoi(const char *__src)
	__attribute__((pure)) __attribute__((__nonnull__(1)));
// Convert a c-string `__src` to an long.
extern long atol(const char *__src)
	__attribute__((pure)) __attribute__((__nonnull__(1)));

// Convert a c-string `__src` to a double and get end pointer `__end`.
extern double strtod(const char *_LCS_REST __src, char **_LCS_REST __end)
	__attribute__((__nonnull__(1)));
// Convert a c-string `__src` to a double and get end pointer `__end`.
extern long strtol(const char *_LCS_REST __src, char **_LCS_REST __end, int __base)
	__attribute__((__nonnull__(1)));
// Convert a c-string `__src` to a double and get end pointer `__end`.
extern unsigned long strtoul(const char *_LCS_REST __src, char **_LCS_REST __end, int __base)
	__attribute__((__nonnull__(1)));

// Return a random integer in [0, RAND_MAX].
extern int rand();
// Add a seed to the random number generator.
extern void srand(unsigned __seed);

// Abort the program, immediately.
extern void abort()
	__attribute__((__noreturn__));

// Register a function to call at program exit.
extern void atexit(void (*__fptr)(void))
	__attribute__((__nonnull__(1)));

// Exit the program.
extern void exit(int __exitcode);

// Get value of environment variable `__env`, or NULL if it does not exist.
extern char *getenv(const char *__env);

// TODO: There are more FUNCTORS.

#ifdef __cplusplus
} // extern "C"
#endif
