# tests/CMakeLists.txt
add_library(testinyjson unit.c)
add_subdirectory("../tinyjson")
target_include_directories(testinyjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(testinyjson tinyjson)