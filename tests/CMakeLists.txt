# tests/CMakeLists.txt
add_library(testinyjson unit.c)
target_include_directories(testinyjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
