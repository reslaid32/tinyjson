# Define the library name
set(LIBRARY_NAME tinyjson)

# Set directories for sources and headers
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Define the source files
set(SRC_FILES ${SRC_DIR}/json.c)
set(HEADER_FILES ${INC_DIR}/tinyjson/json.h ${INC_DIR}/tinyjson/export.h)

# Create the shared library
add_library(${LIBRARY_NAME}_shared SHARED ${SRC_FILES} ${HEADER_FILES})
set_target_properties(${LIBRARY_NAME}_shared PROPERTIES OUTPUT_NAME "tinyjson")

# Create the static library
add_library(${LIBRARY_NAME}_static STATIC ${SRC_FILES} ${HEADER_FILES})
set_target_properties(${LIBRARY_NAME}_static PROPERTIES OUTPUT_NAME "tinyjson")

# Include the headers
target_include_directories(${LIBRARY_NAME}_shared PUBLIC ${INC_DIR})
target_include_directories(${LIBRARY_NAME}_static PUBLIC ${INC_DIR})
