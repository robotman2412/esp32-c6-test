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
project(libbadge LANGUAGES C CXX ASM)

# Compiler options: Exceptions disabled, no system libraries.
add_compile_options(
	-march=rv32imac -mabi=ilp32
	-nostdinc
	-I${CMAKE_CURRENT_LIST_DIR}/../../lib/libcstub/include
	-I${CMAKE_CURRENT_LIST_DIR}/../libbadge/include
	-ffreestanding -fno-exceptions
	-fPIE
)
# Linker options: Relocatable, no system libraries
add_link_options(
	-march=rv32imac -mabi=ilp32
	-nostdlib -nodefaultlibs
	-fPIE
)

# Add ABI files into include path.
include_directories(
	include
)

# Create output file and add sources.
add_library(badge SHARED
	src/system.c
)
