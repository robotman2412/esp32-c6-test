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
#include "stdarg.h"

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

/* ==== MISC DEFINITIONS ==== */

// Returned by some I/O functions when the end of file is reached.
#define EOF (-1)

// Seek with absolute position.
#define SEEK_SET 0
// Seek relative to current address.
#define SEEK_CUR 1
// Seek relative to end of file.
#define SEEK_END 2

// Get type definitions.
#include <bits/types.h>
#include <bits/types/__fpos_t.h>
#include <bits/types/FILE.h>
typedef __fpos_t fpos_t;

// Program input.
extern FILE *__get_stdin();
// Program input.
#define stdin (__get_stdin())

// Program normal output.
extern FILE *__get_stdout();
// Program normal output.
#define stdout (__get_stdout())

// Program error output.
extern FILE *__get_stderr();
// Program error output.
#define stderr (__get_stderr())



/* ==== FILE MANAGEMENT ==== */

// Delete file at `__path`.
extern int remove(const char *__path);
// Rename file at `__old` to `__new`.
extern int rename(const char __old, const char *__new);

// Open a temporary (deleted when `fclose` is called on the return value) file for read/write binary.
extern FILE *tmpfile();
// Create a filename for a temporary file that does not yet exist.
extern char *tmpnam(char *s);

// Close `__fd`.
extern int fclose(FILE *__fd);
// Flush `__fd` or all streams if `__fd` is NULL.
extern int fflush(FILE *__fd);

// Open a file.
extern FILE *fopen(const char *_LCS_REST __path, const char *_LCS_REST __mode);
// Open a file and replace an existing stream with it.
extern FILE *freopen(const char *_LCS_REST __path, const char *_LCS_REST __mode, FILE *_LCS_REST __fd);



/* ==== PRINTF VARIANTS ==== */

// Format `__fmt` and write it to `__fd`.
extern int vfprintf(FILE *_LCS_REST __fd, const char *_LCS_REST __fmt, va_list __vararg);
// Format `__fmt` and write it to `__fd`.
extern int fprintf(FILE *_LCS_REST __fd, const char *_LCS_REST __fmt, ...)
	__attribute__((format(printf, 2, 3)));
// Format `__fmt` and write it to `stdout`.
extern int vprintf(const char *_LCS_REST __fmt, va_list __vararg);
// Format `__fmt` and write it to `stdout`.
extern int printf(const char *_LCS_REST __fmt, ...)
	__attribute__((format(printf, 1, 2)));

// Format `__fmt`, allocate memory and write it to `__ptr`.
extern int vasprintf(char **_LCS_REST __ptr, const char *_LCS_REST __fmt, va_list __vararg);
// Format `__fmt`, allocate memory and write it to `__ptr`.
extern int asprintf(char **_LCS_REST __ptr, const char *_LCS_REST __fmt, ...)
	__attribute__((format(printf, 2, 3)));
// Format `__fmt` and write at most `__len` bytes of it to `__buf`.
extern int vsnprintf(char *_LCS_REST __buf, size_t __len, const char *_LCS_REST __fmt, va_list __vararg);
// Format `__fmt` and write it to `__buf`.
extern int vsprintf(char *_LCS_REST __buf, const char *_LCS_REST __fmt, va_list __vararg);
// Format `__fmt` and write it to `__buf`.
extern int snprintf(char *_LCS_REST __buf, size_t __len, const char *_LCS_REST __fmt, ...)
	__attribute__((format(printf, 3, 4)));
// Format `__fmt` and write it to `__buf`.
extern int sprintf(char *_LCS_REST __buf, const char *_LCS_REST __fmt, ...)
	__attribute__((format(printf, 2, 3)));



/* ==== SCANF VARIANTS ==== */

// Read formatted (`__fmt`) input from stream `__fd`.
extern int vfscanf(FILE *_LCS_REST __fd, const char *_LCS_REST __fmt, va_list __vararg);
// Read formatted (`__fmt`) input from stream `__fd`.
extern int fscanf(FILE *_LCS_REST __fd, const char *_LCS_REST __fmt, ...)
	__attribute__((format(scanf, 2, 3)));

// Read formatted (`__fmt`) input from `stdin`.
extern int vscanf(const char *_LCS_REST __fmt, va_list __vararg);
// Read formatted (`__fmt`) input from `stdin`.
extern int scanf(const char *_LCS_REST __fmt, ...)
	__attribute__((format(scanf, 1, 2)));

// Read formatted (`__fmt`) input from c-string `__mem`.
extern int vsscanf(const char *_LCS_REST __mem, const char *_LCS_REST __fmt, va_list __vararg);
// Read formatted (`__fmt`) input from c-string `__mem`.
extern int sscanf(const char *_LCS_REST __mem, const char *_LCS_REST __fmt, ...)
	__attribute__((format(scanf, 2, 3)));



/* ==== INPUT/OUTPUT ==== */

// Read a c-string `__dst` from an input stream `__fd`.
extern char *fgets(char *_LCS_REST __dst, int __len, FILE *_LCS_REST __fd);
// Read a character from an input stream `__fd`.
extern int fgetc(FILE *__fd);
// Alias to `fgetc`.
static inline int getc(FILE *__fd) { return fgetc(__fd); }
// Read a character from `stdin`.
extern int getchar();
// Read `__count` elements of `__size` each from stream `__fd` into `__ptr`.
extern int fread(void *_LCS_REST __ptr, size_t __size, size_t __count, FILE *_LCS_REST __fd);

// Push a character back into an input stream `__fd`.
extern int ungetc(int __c, FILE *__fd);

// Write a c-string `__buf` to `__fd`.
extern int fputs(const char *_LCS_REST __buf, FILE *_LCS_REST __fd);
// Write a c-string `__buf` followed by a newline to `stdout`.
extern int puts(const char *_LCS_REST __buf);
// Write a character `__c` to a stream `__fd`.
extern int fputc(int __c, FILE *__fd);
// Alias to `fputc`.
static inline int putc(int __c, FILE *__fd) { return fputc(__c, __fd); }
// Write a character `__c` to `stdout`.
extern int putchar(int __c);
// Write `__count` elements of `__size` each from `__ptr` into stream `__fd`.
extern int fwrite(const void *_LCS_REST __ptr, size_t __size, size_t __count, FILE *_LCS_REST __fd);



/* ==== CONTROL FUNCTIONS ==== */

// Get the current position in stream `__fd`.
extern int fgetpos(FILE *_LCS_REST __stream, fpos_t *_LCS_REST __pos);
// Seek to an absolute position in stream `__fd`.
extern int fsetpos(FILE *_LCS_REST __stream, const fpos_t *_LCS_REST __pos);
// Seek to a position `__pos` in stream `__fd`.
extern int fseek(FILE *__stream, long __off, int __whence);
// Get the current position in stream `__fd`.
extern long ftell(FILE *__stream);
// Rewind to the beginning of stream `__fd`.
extern void rewind(FILE *__stream);

// Set stream `__fd` buffer to `__buf`, size `BUFSIZ`.
extern void setbuf(FILE *_LCS_REST __fd, char *_LCS_REST __buf);
// Set stream `__fd` buffer to `__buf`, size `__len`.
extern void setvbuf(FILE *_LCS_REST __fd, char *_LCS_REST __buf, int __modes, size_t __len);

// Turn off the error indicators of stream `__fd`.
extern void clearerr(FILE *__stream);
// Return the EOF indicator for stream `__fd`.
extern int feof(FILE *__stream);
// Return the error indicator for stream `__fd`.
extern int ferror(FILE *__stream);

// Print an error message `__msg` according to `errno`.
extern void perror(const char *__msg);

#ifdef __cplusplus
} // extern "C"
#endif
