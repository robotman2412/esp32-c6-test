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

#include <malloc.h>
#include <stdio.h>
#include <string.h>

namespace abi {

// Called when an app aborts.
void appAborted() {
	printf("App aborted!\n");
	abort();
}

// Exports ABI symbols into `map`.
void exportSymbols(elf::SymMap &map) {
	// From malloc.h:
	map["malloc"]  = (size_t) &malloc;
	map["free"]    = (size_t) &free;
	map["calloc"]  = (size_t) &calloc;
	map["realloc"] = (size_t) &realloc;
	
	// From stdio.h:
	map["__get_stdin"]  = (size_t) +[]{ return stdin; };
	map["__get_stdout"] = (size_t) +[]{ return stdout; };
	map["__get_stderr"] = (size_t) +[]{ return stderr; };
	map["remove"]       = (size_t) &remove;
	map["rename"]       = (size_t) &rename;
	map["tmpfile"]      = (size_t) &tmpfile;
	map["fclose"]       = (size_t) &fclose;
	map["fflush"]       = (size_t) &fflush;
	map["fopen"]        = (size_t) &fopen;
	map["freopen"]      = (size_t) &freopen;
	map["vfprintf"]     = (size_t) &vfprintf;
	map["fprintf"]      = (size_t) &fprintf;
	map["vprintf"]      = (size_t) &vprintf;
	map["printf"]       = (size_t) &printf;
	map["vasprintf"]    = (size_t) &vasprintf;
	map["asprintf"]     = (size_t) &asprintf;
	map["vsnprintf"]    = (size_t) &vsnprintf;
	map["vsprintf"]     = (size_t) &vsprintf;
	map["snprintf"]     = (size_t) &snprintf;
	map["sprintf"]      = (size_t) &sprintf;
	map["fputs"]        = (size_t) &fputs;
	map["puts"]         = (size_t) &puts;
	map["ungetc"]       = (size_t) &ungetc;
	map["fread"]        = (size_t) &fread;
	map["fwrite"]       = (size_t) &fwrite;
	map["fseek"]        = (size_t) &fseek;
	map["ftell"]        = (size_t) &ftell;
	map["rewind"]       = (size_t) &rewind;
	map["clearerr"]     = (size_t) &clearerr;
	map["feof"]         = (size_t) &feof;
	map["ferror"]       = (size_t) &ferror;
	map["perror"]       = (size_t) &perror;
	
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

} // namespace abi
