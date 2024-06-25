CC = gcc
CFLAGS = -pedantic -Wall -Wextra -O3 -std=c17
LIB = -dynamiclib

libadf.dylib: adf.o crc.o lookup_table.o
	$(CC) $(CFLAGS) $(LIB) -o $@ $^

adf.o: adf.c
	$(CC) $(CFLAGS) -c $^

crc.o: crc.c
	$(CC) $(CFLAGS) -c $^

lookup_table.o: lookup_table.c
	$(CC) $(CFLAGS) -c $^

clean: adf.o crc.o lookup_table.o
	rm $^
