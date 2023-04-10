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

#include "stdio.h"

FILE *__get_stdin() {}
FILE *__get_stdout() {}
FILE *__get_stderr() {}

int remove(const char *__path) { return 0; }
int rename(const char __old, const char *__new) { return 0; }
FILE *tmpfile() { return NULL; }
int fclose(FILE *__fd) { return 0; }
int fflush(FILE *__fd) { return 0; }
FILE *fopen(const char *_LCS_REST __path, const char *_LCS_REST __mode) { return NULL; }
FILE *freopen(const char *_LCS_REST __path, const char *_LCS_REST __mode, FILE *_LCS_REST __fd) { return NULL; }
int vfprintf(FILE *_LCS_REST __fd, const char *_LCS_REST __fmt, va_list __vararg) { return 0; }
int fprintf(FILE *_LCS_REST __fd, const char *_LCS_REST __fmt, ...) { return 0; }
int vprintf(const char *_LCS_REST __fmt, va_list __vararg) { return 0; }
int printf(const char *_LCS_REST __fmt, ...) { return 0; }
int vasprintf(char **_LCS_REST __ptr, const char *_LCS_REST __fmt, va_list __vararg) { return 0; }
int asprintf(char **_LCS_REST __ptr, const char *_LCS_REST __fmt, ...) { return 0; }
int vsnprintf(char *_LCS_REST __buf, size_t __len, const char *_LCS_REST __fmt, va_list __vararg) { return 0; }
int vsprintf(char *_LCS_REST __buf, const char *_LCS_REST __fmt, va_list __vararg) { return 0; }
int snprintf(char *_LCS_REST __buf, size_t __len, const char *_LCS_REST __fmt, ...) { return 0; }
int sprintf(char *_LCS_REST __buf, const char *_LCS_REST __fmt, ...) { return 0; }
int fscanf(FILE *_LCS_REST __fd, const char *_LCS_REST __fmt, ...) { return 0; }
int scanf(const char *_LCS_REST __fmt, ...) { return 0; }
int sscanf(const char *_LCS_REST __mem, const char *_LCS_REST __fmt, ...) {return 0; }
int fputs(const char *_LCS_REST __buf, FILE *_LCS_REST __fd) { return 0; }
int puts(const char *_LCS_REST __buf) { return 0; }
int ungetc(int __c, FILE *__fd) { return 0; }
int fread(void *_LCS_REST __ptr, size_t __size, size_t __count, FILE *_LCS_REST __fd) { return 0; }
int fwrite(const void *_LCS_REST __ptr, size_t __size, size_t __count, FILE *_LCS_REST __fd) { return 0; }
int fseek(FILE *__stream, long __off, int __whence) { return 0; }
long ftell(FILE *__stream) { return 0; }
void rewind(FILE *__stream) {}
void clearerr(FILE *__stream) {}
int feof(FILE *__stream) { return 0; }
int ferror(FILE *__stream) { return 0; }
void perror(const char *__msg) {}

int __printf_chk (int flag, const char *format, ...) {}
