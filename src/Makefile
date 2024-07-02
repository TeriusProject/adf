# src/Makefile
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
AR = ar
CFLAGS = -pedantic -Wall -Wextra -O3 -std=c2x -fPIC
OBJS = adf.o crc.o lookup_table.o
LIB_NAME = libadf.a
INCLUDE = /usr/local/include
LIB = /usr/local/lib

$(LIB_NAME): $(OBJS)
	$(AR) cr $@ $^

adf.o: adf.c
	$(CC) $(CFLAGS) -c $^

crc.o: crc.c
	$(CC) $(CFLAGS) -c $^

lookup_table.o: lookup_table.c
	$(CC) $(CFLAGS) -c $^

.PHONY : clean
clean:
	rm -f $(OBJS) $(LIB_NAME)

.PHONY : install
# currently just install on macOS and GNU/Linux
install: adf.h $(LIB_NAME)
	cp $< $(INCLUDE)
	cp $(LIB_NAME) $(LIB)
