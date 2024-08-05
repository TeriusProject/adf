CC = g++
CXXFLAGS = -pedantic -Wall -Wextra -fPIC -std=c++20 -O3
INCLUDE = /usr/local/include
LIB_DIR = /usr/local/lib
LIB = libadfpp.dylib
OBJS = adf.o header.o series.o

$(LIB): $(OBJS)
	$(CC) $(CXXFLAGS) -dynamiclib -ladf -o $@ $^ -I $(INCLUDE) -L $(LIB_DIR)

adf.o: adf.cc adf.hpp exceptions.hpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

header.o: header.cc adf.hpp exceptions.hpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

series.o: series.cc adf.hpp exceptions.hpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

.PHONY: install
install: adf.hpp $(LIB)
	cp $< $(INCLUDE)
	cp $(LIB) $(LIB_DIR)

.PHONY: example
example: example.cc
	$(CC) $(CXXFLAGS) -ladfpp -o $@ $<

.PHONY: clean
clean:
	rm -f $(LIB) $(OBJS)
