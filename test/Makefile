CC = gcc
CFLAGS = -pedantic -Wall -Wextra -O3 -std=c17
SRC = ../src/
BIN = generate_sample test_create test_marshal test_unmarshal test_update test_lookup_table

all: $(BIN)
	./test_create
	./test_marshal
	./test_unmarshal
	./test_update
	./test_lookup_table

test_create: test_create.c test.c mock.c $(SRC)adf.c  $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@  

test_marshal: test_marshal.c test.c mock.c $(SRC)adf.c  $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@  

test_unmarshal: test_unmarshal.c test.c mock.c $(SRC)adf.c  $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@  

test_update: test_update.c test.c mock.c $(SRC)adf.c  $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@  

test_lookup_table: test_lookup_table.c test.c $(SRC)adf.c  $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@ 

generate_sample: generate_sample.c mock.c $(SRC)adf.c  $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^ -o $@

clean: $(BIN)
	rm $^
