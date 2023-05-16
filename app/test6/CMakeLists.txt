cmake_minimum_required(VERSION 3.10)

# Import BadgeSDK.
include(import.cmake)

# Define project name.
project(test6)
set(target main6.o)

# Add sources.
add_executable(${target}
	src/main.c
)
target_include_directories(${target} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/../../lib/pax-graphics/pax-graphics/src
)
target_link_libraries(${target} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/../../lib/pax-graphics/build/libpax.so
	m display
)

# Set compile options.
badgesdk_define_app(${target})
