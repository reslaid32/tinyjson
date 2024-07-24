LIBRARY_NAME = tinyjson

SRC_FILES = json.c
HEADER_FILES = json.h _export.h

lib$(LIBRARY_NAME).a: $(SRC_FILES)
	@echo "Compiling static library..."
	$(CC) -c $(SRC_FILES) $(CFLAGS)
	ar rcs lib$(LIBRARY_NAME).a $(SRC_FILES:.c=.o)

lib$(LIBRARY_NAME).so: $(SRC_FILES)
	@echo "Compiling shared library..."
	$(CC) -fPIC -shared -o lib$(LIBRARY_NAME).so $(SRC_FILES) $(CFLAGS)

clean:
	rm -f lib$(LIBRARY_NAME).a lib$(LIBRARY_NAME).so $(SRC_FILES:.c=.o)

.PHONY: clean
