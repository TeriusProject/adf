# test/Makefile
# ------------------------------------------------------------------------
# ADF - Agriculture Data Format
# Copyright (C) 2024 Matteo Nicoli
#
# This file is part of Terius
#
# ADF is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# ADF is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

CC = gcc
CFLAGS = -pedantic -Wall -Wextra -O3 -std=c2x
SRC = ../src/
ADF_SOURCE = $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
BIN = test_create test_reindex test_marshal test_unmarshal test_series_add \
	  test_series_update test_series_remove test_lookup_table test_copy    \
	  test_free

all: $(BIN) sample.adf
	@echo "*****************************\n  Executing tests\n*****************************"
	./test_create
	./test_reindex
	./test_marshal
	./test_unmarshal
	./test_series_add
	./test_series_update
	./test_series_remove
	./test_copy
	./test_lookup_table
	./test_free

test_create: test_create.c test.c mock.c $(ADF_SOURCE)
	$(CC) $(CFLAGS) $^  -o $@  

test_reindex: test_reindex.c test.c mock.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@  

test_marshal: test_marshal.c test.c mock.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@  

test_unmarshal: test_unmarshal.c test.c mock.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@  

test_series_add: test_series_add.c test.c mock.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@ 

test_series_update: test_series_update.c test.c mock.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@

test_series_remove: test_series_remove.c test.c mock.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@

test_copy: test_copy.c test.c mock.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@ 

test_free: test_free.c test.c mock.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@ 

test_lookup_table: test_lookup_table.c test.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^  -o $@

generate_sample: generate_sample.c mock.c $(SRC)adf.c $(SRC)crc.c $(SRC)lookup_table.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: init
init: generate_sample
	./generate_sample

.PHONY: clean
clean:
	rm -f $(BIN) generate_sample
