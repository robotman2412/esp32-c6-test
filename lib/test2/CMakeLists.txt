cmake_minimum_required(VERSION 3.10)

# Compile in release mode by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Set the C and C++ compilers to use.
set(CMAKE_C_COMPILER "riscv64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "riscv64-linux-gnu-g++")
# These don't pass built-in cmake test, so disable said test.
set(CMAKE_C_COMPILER_FORCED true)
set(CMAKE_CXX_COMPILER_FORCED true)

# Define the project.
project(test2 LANGUAGES C CXX ASM)

# Compiler options: Exceptions disabled, no system libraries.
add_compile_options(-march=rv32imac -mabi=ilp32 -ffreestanding -fPIC)
# Linker options: Relocatable, no system libraries
add_link_options(-march=rv32imac -mabi=ilp32 -nostdlib -nodefaultlibs -fPIC)

# Add ABI files into include path.
include_directories(
	src
	../libcstub/include
	../libcstub/include_stubs
	../libbadge/include
)

# Create output file and add sources.
add_library(test2 SHARED
	src/main.c
)
