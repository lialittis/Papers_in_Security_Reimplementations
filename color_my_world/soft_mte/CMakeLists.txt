cmake_minimum_required(VERSION 3.10)
project(soft_mte)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_CXX_FLAGS_DEBUG "-g")

include_directories(include)

# Create a shared library for tagged allocator
add_library(tag_allocator SHARED src/tag_allocator.cpp)
# target_compile_options(tag_allocator PRIVATE)

# Create a shared library for tag checker
add_library(tag_checker SHARED src/tag_checker.cpp)
target_link_libraries(tag_checker PRIVATE tag_allocator dl)
target_compile_options(tag_checker PRIVATE -fno-builtin)

# For tag_checker library
set_target_properties(tag_checker PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
)

# Test executable
add_executable(test_memory test/test_main.cpp)
target_link_libraries(test_memory PRIVATE tag_checker tag_allocator)
target_compile_definitions(test_memory PRIVATE TESTING)
target_compile_options(test_memory PRIVATE -g -fno-builtin)
