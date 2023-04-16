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

#include <stdint.h>
#include <stddef.h>

namespace __subcxx {

template <typename _T>
class __reverse_iter {
	private:
		// Wrapped iterator.
		_T __value;
		
	public:
		// Makes a reverse edition of another iterator.
		template<typename... _Args>
		__reverse_iter(_Args... __args): _T(__args...) {}
		
		// Move forward (prefix).
		__reverse_iter &operator++() {
			--__value;
			return *this;
		}
		// Move forward (postfix).
		__reverse_iter operator++(int) {
			auto tmp = __reverse_iter(__value);
			--__value;
			return tmp;
		}
		// Additive.
		__reverse_iter operator+(int __off) {
			return __reverse_iter(__value - __off);
		}
		// Additive.
		__reverse_iter &operator+=(int __off) {
			__value -= __off;
			return *this;
		}
		
		// Move backward (prefix).
		__reverse_iter &operator--() {
			++__value;
			return *this;
		}
		// Move backward (postfix).
		__reverse_iter operator--(int) {
			auto tmp = __reverse_iter(__value);
			++__value;
			return tmp;
		}
		// Additive.
		__reverse_iter operator-(int __off) {
			return __reverse_iter(__value + __off);
		}
		// Additive.
		__reverse_iter &operator-=(int __off) {
			__value += __off;
			return *this;
		}
		
		// Difference.
		int operator-(__reverse_iter &__other) {
			return __other.__value - __value;
		}
		
		// Indexing.
		_T &operator*() {
			return *__value;
		}
		// Indexing.
		_T &operator->() {
			return *__value;
		}
		// Indexing.
		_T &operator[](int __off) {
			return __value[-__off];
		}
};

};
