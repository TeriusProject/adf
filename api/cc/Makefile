CC = g++
CXXFLAGS = -pedantic -Wall -Wextra -fPIC -std=c++20 -O3
INCLUDE = /usr/local/include
LIB_DIR = /usr/local/lib
LIB = libadfpp.dylib

$(LIB): adf.cc adf.hpp exceptions.hpp
	$(CC) $(CXXFLAGS) $< -dynamiclib -ladf -o $@ -I $(INCLUDE) -L $(LIB_DIR)

.PHONY: install
install: adf.hpp $(LIB)
	cp $< $(INCLUDE)
	cp $(LIB) $(LIB_DIR)

.PHONY: example
example: example.cc
	DYLD_INSERT_LIBRARIES=$(LIB_DIR) $(CC) $(CXXFLAGS)  -ladfpp   -o $@ $<

.PHONY: clean
clean: 
	rm -f adfpp.o libadfpp.dylib
