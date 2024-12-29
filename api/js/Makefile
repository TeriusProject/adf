# api/js/Makefile
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
# Terius is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

CC = emcc
CARGS = -pedantic -Wall -Wextra -std=c2x -fPIC
EMCCARGS = --no-entry -sSTANDALONE_WASM
WASM_OUT = adflib.wasm
EXPORTS = -sEXPORTED_FUNCTIONS=_malloc,_free,_size_header,_size_adf_t
SOURCES = ../../src/adf.c ../../src/crc.c ../../src/lookup_table.c

$(WASM_OUT): $(SOURCES)
	$(CC) $(CARGS) $^ -o $@ $(EMCCARGS) $(EXPORTS)

.PHONY: clean
clean: $(WASM_OUT)
	rm -f $^
