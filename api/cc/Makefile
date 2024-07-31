CC = g++
CXXFLAGS = -pedantic -Wall -Wextra -fPIC -std=c++20 -O3
INCLUDE = /usr/local/include
LIB = /usr/local/lib
OBJ = adf

all: $(OBJ)

$(OBJ): adf.cc adf.hpp exceptions.hpp utils.hpp
	$(CC) $(CXXFLAGS) $< -ladf -o $(OBJ) -I $(INCLUDE) -L $(LIB)

clean: $(OBJ)
	rm -f $^
