cmake_minimum_required(VERSION 3.10)

# Compile in release mode by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Define the project.
project(elfloader LANGUAGES C CXX ASM)

# Add ABI files into include path.
include_directories(
	src
)

# Create output file and add sources.
add_library(elfloader
	src/dynlinker.cpp
	src/elfloader.cpp
)