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

#include "string.h"

void  *memchr    (const void *__mem, int __c, size_t __len) { return 0; }
void  *memrchr   (const void *__mem, int __c, size_t __len) { return 0; }
int    memcmp    (const void *__a, const void *__b, size_t __len) { return 0; }
void  *memccpy   (void *_LCS_REST __dst, const void *_LCS_REST __src, int __c, size_t __len) { return 0; }
void  *memcpy    (void *_LCS_REST __dst, const void *_LCS_REST __src, size_t __len) { return 0; }
void  *memmove   (void *__dst, const void *__src, size_t __len) { return 0; }
void  *memset    (void *__dst, int __c, size_t __len) { return 0; }
char  *strchr    (const char *__mem, int __c) { return NULL; }
char  *strrchr   (const char *__mem, int __c) { return NULL; }
int    strcmp    (const char *__a, const char *__b) { return 0; }
int    strncmp   (const char *__a, const char *__b, size_t __len) { return 0; }
char  *strcat    (char *__dst, const char *__src) { return NULL; }
char  *strncat   (char *__dst, const char *__src, size_t __len) { return 0; }
size_t strspn    (const char *__mem, const char *__accept) { return 0; }
size_t strcspn   (const char *__mem, const char *__reject) { return 0; }
char  *strstr    (const char *__mem, const char *__substr) { return NULL; }
char  *strcasestr(const char *__mem, const char *__substr) { return NULL; }
size_t strlen    (const char *__mem) { return 0; }
size_t strnlen   (const char *__mem, size_t __len) { return 0; }
char  *strerror  (int __code) { return NULL; }
