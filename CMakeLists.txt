cmake_minimum_required(VERSION 3.5)
project(tinyjson)

# Set output directory for all binaries (libs and executables)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add subdirectories
add_subdirectory(source)

add_executable(main main.c)
target_link_libraries(main tinyjson_shared)
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)