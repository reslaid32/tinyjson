EXECUTABLE = main

LIB_DIR = tinyjson

CFLAGS = -Wall -Wextra -I$(LIB_DIR)
LDFLAGS = -L$(LIB_DIR) -ltinyjson

$(EXECUTABLE): $(EXECUTABLE).c
	@echo "Compiling main executable..."
	$(CC) $(CFLAGS) $(EXECUTABLE).c -o $(EXECUTABLE) $(LDFLAGS)

all: $(LIB_DIR)/libtinyjson.so $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

clean-lib:
	cd $(LIB_DIR) && $(MAKE) clean

clean-all: clean clean-lib

.PHONY: all clean clean-lib clean-all
