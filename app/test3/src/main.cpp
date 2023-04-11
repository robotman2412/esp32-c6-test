
#include <stdio.h>

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <bitset>
#include <cassert>
#include <ccomplex>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
// Not working:
// #include <charconv>
#include <chrono>
#include <cinttypes>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <codecvt>
#include <compare>
#include <complex>
#include <concepts>
#include <condition_variable>
// Requires -fcoroutine:
// #include <coroutine>
#include <csetjmp>
#include <csignal>
#include <cstdalign>
#include <cstdarg>
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
// Missing functions:
// #include <cstring>
#include <ctgmath>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <exception>
#include <execution>
#include <filesystem>
#include <forward_list>
// Missing defines:
// #include <fstream>
#include <functional>
#include <future>
#include <initializer_list>
#include <iomanip>
#include <ios>
#include <iosfwd>
// Missing linkage:
// #include <iostream>
#include <istream>
#include <iterator>
#include <latch>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
#include <queue>
// Missing types:
// #include <random>
#include <ranges>
#include <ratio>
// Missing functions via <cstring>:
// #include <regex>
#include <scoped_allocator>
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <stop_token>
#include <streambuf>
#include <string>
#include <string_view>
#include <syncstream>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>
#include <version>

struct X {
	X() {
		printf("X()\n");
	}
	~X() {
		printf("~X()\n");
	}
};

X myThingy;

int main(int argc, char **argv, char **envp) {
	for (int i = 0; i < argc; i++) {
		printf("Arg %d: %s\n", i, argv[i]);
	}
	for (int i = 0; envp[i]; i++) {
		printf("Env %d: %s\n", i, envp[i]);
	}
	return 0xcafe;
}
