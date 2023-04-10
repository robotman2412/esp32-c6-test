cmake_minimum_required(VERSION 3.10)

# Compile in release mode by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# Set the C and C++ compilers to use.
set(CMAKE_C_COMPILER "riscv64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "riscv64-linux-gnu-g++")
# These don't pass built-in cmake test, so disable said test.
set(CMAKE_C_COMPILER_FORCED true)
set(CMAKE_CXX_COMPILER_FORCED true)

# Define the project.
project(test1 LANGUAGES C CXX ASM)

# Compiler options: Exceptions disabled, no system libraries.
add_compile_options(
	-march=rv32imac -mabi=ilp32
	-nostdinc
	-I${CMAKE_CURRENT_LIST_DIR}/../../lib/libcstub/include
	-I/usr/lib/gcc-cross/riscv64-linux-gnu/9/include
	# -I/usr/riscv64-linux-gnu/include/c++/9
	# -I/usr/riscv64-linux-gnu/include/c++/9/riscv64-linux-gnu
	# -I/usr/riscv64-linux-gnu/include/c++/9/backward
	# -I/usr/include
	-ffreestanding -fno-exceptions
	-fPIE
	-fdata-sections -ffunction-sections
)
# Linker options: Relocatable, no system libraries
add_link_options(
	-march=rv32imac -mabi=ilp32
	-nostdlib -nodefaultlibs
	-fPIE
	-Wl,--gc-sections
	-T${CMAKE_CURRENT_LIST_DIR}/linker.ld
)

# Add ABI files into include path.
include_directories(
	src
)


# Create output file and add sources.
add_executable(main2.o
	src/entry.c
	src/main.cpp
)

# Link the libraries yes.
target_link_libraries(main2.o ${CMAKE_CURRENT_LIST_DIR}/../../lib/libcstub/build/libc.so)

