# tinyjson/CMakeLists.txt
add_library(tinyjson json.c)
target_include_directories(tinyjson PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
