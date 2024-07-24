# Define the library name
set(LIBRARY_NAME tinyjson)

# Define the source files
set(SRC_FILES json.c)
set(HEADER_FILES json.h _export.h)

# Create the shared library
add_library(${LIBRARY_NAME}_shared SHARED ${SRC_FILES} ${HEADER_FILES})
set_target_properties(${LIBRARY_NAME}_shared PROPERTIES OUTPUT_NAME "tinyjson")

# Create the static library
add_library(${LIBRARY_NAME}_static STATIC ${SRC_FILES} ${HEADER_FILES})
set_target_properties(${LIBRARY_NAME}_static PROPERTIES OUTPUT_NAME "tinyjson")

# Include the headers
target_include_directories(${LIBRARY_NAME}_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${LIBRARY_NAME}_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
