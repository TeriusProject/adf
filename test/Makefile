CC = gcc
CFLAGS = -pedantic -Wall -Wextra -O3 -std=c17

all: test_create test_marshal test_unmarshal test_update test_lookup_table

test_create: test_create.c test.c mock.c
	$(CC) $(CFLAGS) -ladf $^  -o $@  

test_marshal: test_marshal.c test.c mock.c
	$(CC) $(CFLAGS) -ladf $^  -o $@  

test_unmarshal: test_unmarshal.c test.c mock.c
	$(CC) $(CFLAGS) -ladf $^  -o $@  

test_update: test_update.c test.c mock.c
	$(CC) $(CFLAGS) -ladf $^  -o $@  

test_lookup_table: test_lookup_table.c test.c
	$(CC) $(CFLAGS) -ladf $^  -o $@ 

clean: test_create test_marshal test_unmarshal test_update test_lookup_table
	rm $^
