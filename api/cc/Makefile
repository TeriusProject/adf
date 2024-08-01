CC = g++
CXXFLAGS = -pedantic -Wall -Wextra -fPIC -std=c++20 -O3
INCLUDE = /usr/local/include
LIB_DIR = /usr/local/lib
LIB = libadfpp.dylib

$(LIB): adf.cc adf.hpp exceptions.hpp utils.hpp
	$(CC) $(CXXFLAGS) $< -dynamiclib -ladf -o $@ -I $(INCLUDE) -L $(LIB_DIR)

.PHONY: clean
clean: 
	rm -f adfpp.o libadfpp.dylib

.PHONY: install
install: adf.hpp $(LIB)
	cp $< $(INCLUDE)
	cp $(LIB) $(LIB_DIR)
