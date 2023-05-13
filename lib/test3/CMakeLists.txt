cmake_minimum_required(VERSION 3.10)

# Import BadgeSDK.
include(import.cmake)

# Define project name.
project(libtest3)
set(target test3)

# Add sources.
add_library(${target} SHARED
	src/main.c
)

# Set compile options.
badgesdk_define_shared_lib(${target})
