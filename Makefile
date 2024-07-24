LIBRARY_NAME = tinyjson
SHARED_LIB = lib$(LIBRARY_NAME).so
STATIC_LIB = lib$(LIBRARY_NAME).a
EXECUTABLE = main
TEST_EXECUTABLE = test_json

SRC_FILES = json.c
HEADER_FILES = json.h _export.h
TEST_SRC = unit.c

CC = gcc
CFLAGS = -Wall -Wextra -Itinyjson
LDFLAGS = -L. -l$(LIBRARY_NAME) -Wl,-rpath,.

all: $(SHARED_LIB) $(STATIC_LIB) $(EXECUTABLE) $(TEST_EXECUTABLE)

$(SHARED_LIB): $(SRC_FILES) $(HEADER_FILES)
	$(CC) $(CFLAGS) -shared -o $@ $(SRC_FILES)

$(STATIC_LIB): $(SRC_FILES) $(HEADER_FILES)
	ar rcs $@ $(SRC_FILES:.c=.o)

$(EXECUTABLE): main.c $(SHARED_LIB)
	$(CC) $(CFLAGS) -o $@ main.c $(LDFLAGS)

$(TEST_EXECUTABLE): $(TEST_SRC) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(TEST_SRC) $(LDFLAGS)

check: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

clean:
	rm -f $(SHARED_LIB) $(STATIC_LIB) $(EXECUTABLE) $(TEST_EXECUTABLE) *.o

.PHONY: all clean check
