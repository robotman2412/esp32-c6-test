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

#include <subcxxbits/reverse_iter.hpp>

namespace __subcxx {

template<typename _T>
class __arraytype {
	public:
		using iterator = _T*;
		using consr_iterator = const _T*;
		using reverse_iterator = __reverse_iter<_T*>;
		using const_reverse_iterator = __reverse_iter<const _T*>;
		
		
		// Get data stub.
		const _T* data() const { return nullptr; }
		// Get data stub.
		_T* data() { return nullptr; }
		// Get size stub.
		size_t size() const { return 0; }
		// Is empty?
		bool empty() const { return !size(); }
		
		
		// Get forward iterator.
		auto begin() { return iterator(data()); }
		// Get forward iterator.
		auto end() { return iterator(data() + size()); }
		
		// Get forward iterator.
		auto rbegin() { return reverse_iterator(data() + size() - 1); }
		// Get forward iterator.
		auto rend() { return reverse_iterator(data() - 1); }
		
		// Get forward iterator.
		auto begin() const { return const_iterator(data()); }
		// Get forward iterator.
		auto end() const { return const_iterator(data() + size()); }
		// Get forward iterator.
		auto cbegin() const { return const_iterator(data()); }
		// Get forward iterator.
		auto cend() const { return const_iterator(data() + size()); }
		
		// Get forward iterator.
		auto crbegin() const { return const_reverse_iterator(data() + size() - 1); }
		// Get forward iterator.
		auto crend() const { return const_reverse_iterator(data() - 1); }
		
		
		// Get reference to front character.
		_T &front() { return *data(); }
		// Get reference to front character.
		const _T &front() const { return *data(); }
		// Get reference to front character.
		_T &back() { return data()[size()-1]; }
		// Get reference to front character.
		const _T &back() const { return data()[size()-1]; }
};

};
