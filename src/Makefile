CC = gcc
AR = ar
CFLAGS = -pedantic -Wall -Wextra -O3 -std=c17 -fPIC

libadf.a: adf.o crc.o lookup_table.o
	$(AR) cr $@ $^

adf.o: adf.c
	$(CC) $(CFLAGS) -c $^

crc.o: crc.c
	$(CC) $(CFLAGS) -c $^

lookup_table.o: lookup_table.c
	$(CC) $(CFLAGS) -c $^

clean: adf.o crc.o lookup_table.o
	rm $^
