# Compiler
CC = clang

# Directories
SRC_DIR = tinyjson
TEST_DIR = tests
BUILD_DIR = build

# Source files
SRC_FILES = $(SRC_DIR)/json.c $(TEST_DIR)/unit.c main.c

# Include directories
INCLUDES = -I$(SRC_DIR)

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Output executable
TARGET = $(BUILD_DIR)/tinyjson

# Make rules
all: $(TARGET)

$(TARGET): $(SRC_FILES)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $(SRC_FILES) -o $(TARGET)

check:
	${TARGET}

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
