cmake_minimum_required(VERSION 3.10)

# Import BadgeSDK.
include(import.cmake)

# Define project name.
project(pax_graphics C CXX)
add_definitions(-DPAX_STANDALONE=1)
add_definitions(-DPAX_COMPILE_MCR=0)
# set(PAX_COMPILE_CXX)
include(pax-graphics/Standalone.cmake)

# Set compile options.
badgesdk_define_static_lib(pax_graphics)
