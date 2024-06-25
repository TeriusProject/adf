CC = g++
CFLAGS = -Wall -Wextra -fPIC -std=c++17 -O3
INCLUDE = ../../src
LIB = ../../src
OBJ = adf.o

all: $(OBJ)

adf.o: adf.cc
	$(CC) $(CFLAGS) -c $^ -ladf -o $@ -I $(INCLUDE) -L $(LIB)

clear: $(OBJ)
	rm -f $^
