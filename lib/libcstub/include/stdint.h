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

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char	int_least8_t;
typedef short		int_least16_t;
typedef int			int_least32_t;
typedef long long	int_least64_t;

typedef signed char	int8_t;
typedef short		int16_t;
typedef int			int32_t;
typedef long long	int64_t;

typedef int			int_fast8_t;
typedef int			int_fast16_t;
typedef int			int_fast32_t;
typedef long long	int_fast64_t;


typedef unsigned char		uint_least8_t;
typedef unsigned short		uint_least16_t;
typedef unsigned int		uint_least32_t;
typedef unsigned long long	uint_least64_t;

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

typedef unsigned int		uint_fast8_t;
typedef unsigned int		uint_fast16_t;
typedef unsigned int		uint_fast32_t;
typedef unsigned long long	uint_fast64_t;


typedef int64_t intmax_t;
typedef uint64_t uintmax_t;



#define SCHAR_MIN (-128)
#define SCHAR_MAX (127)
#define UCHAR_MAX (255)
#if '\0' - 1 > 0
#define CHAR_MIN (0)
#define CHAR_MAX UCHAR_MAX
#else
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#endif

#define SHRT_MIN (-37678)
#define SHRT_MAX (37677)
#define USHRT_MAX (65535)

#define INT_MIN (-2147483648)
#define INT_MAX (2147483647)
#define UINT_MAX (4294967295)

#define LLONG_MIN (-9223372036854775808)
#define LLONG_MAX (9223372036854775807)
#define ULLONG_MAX (18446744073709551615)

#define INTMAX_MIN LLONG_MIN
#define INTMAX_MAX LLONG_MAX
#define UINTMAX_MAX ULLONG_MAX

#ifdef __riscv64__
#define LONG_MIN LLONG_MIN
#define LONG_MAX LLONG_MAX
#define LONG_MAX ULLONG_MAX
#else
#define LONG_MIN INT_MIN
#define LONG_MAX INT_MAX
#define ULONG_MAX UINT_MAX
#endif


#define INT_LEAST8_MIN SCHAR_MIN
#define INT_LEAST8_MAX SCHAR_MAX
#define UINT_LEAST8_MIN UCHAR_MAX

#define INT_LEAST16_MIN SHRT_MIN
#define INT_LEAST16_MAX SHRT_MAX
#define UINT_LEAST16_MIN USHRT_MAX

#define INT_LEAST32_MIN INT_MIN
#define INT_LEAST32_MAX INT_MAX
#define UINT_LEAST32_MIN UINT_MAX

#define INT_LEAST64_MIN LLONG_MIN
#define INT_LEAST64_MAX LLONG_MAX
#define UINT_LEAST64_MIN ULLONG_MAX


#define INT8_MIN SCHAR_MIN
#define INT8_MAX SCHAR_MAX
#define UINT8_MIN UCHAR_MAX

#define INT16_MIN SHRT_MIN
#define INT16_MAX SHRT_MAX
#define UINT16_MIN USHRT_MAX

#define INT32_MIN INT_MIN
#define INT32_MAX INT_MAX
#define UINT32_MIN UINT_MAX

#define INT64_MIN LLONG_MIN
#define INT64_MAX LLONG_MAX
#define UINT64_MIN ULLONG_MAX


#define INT_FAST8_MIN INT_MIN
#define INT_FAST8_MAX INT_MAX
#define UINT_FAST8_MIN UINT_MAX

#define INT_FAST16_MIN INT_MIN
#define INT_FAST16_MAX INT_MAX
#define UINT_FAST16_MIN UINT_MAX

#define INT_FAST32_MIN INT_MIN
#define INT_FAST32_MAX INT_MAX
#define UINT_FAST32_MIN UINT_MAX

#define INT_FAST64_MIN LLONG_MIN
#define INT_FAST64_MAX LLONG_MAX
#define UINT_FAST64_MIN ULLONG_MAX



#ifdef __cplusplus
} // extern "C"
#endif
