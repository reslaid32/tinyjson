cmake_minimum_required(VERSION 3.10)
project(main)

# Set the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Source and include directories
include_directories("tinyjson")
add_subdirectory("tinyjson")

# Main executable
add_executable(main main.c)
add_library(testinyjson "tests/unit.c")

# Link the tinyjson library and test files
target_include_directories(testinyjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(testinyjson tinyjson)
target_link_libraries(main testinyjson)
