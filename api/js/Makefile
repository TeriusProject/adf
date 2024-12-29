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
EXPORTS = -sEXPORTED_FUNCTIONS=_malloc,_free,_size_header,_size_adf_t \
		  -sEXPORTED_FUNCTIONS=_create_header,_create_additive,_create_wavelength_info, \
		  -sEXPORTED_FUNCTIONS=_create_reduction_info,_create_soil_depth_info, \
		  -sEXPORTED_FUNCTIONS=_create_trans_soil_depth_info,_create_precision_info \
		  -sEXPORTED_FUNCTIONS=_get_status_code_OK,_get_status_code_HEADER_CORRUPTED \
		  -sEXPORTED_FUNCTIONS=_get_status_code_METADATA_CORRUPTED,_get_status_code_SERIES_CORRUPTED \
		  -sEXPORTED_FUNCTIONS=_get_status_code_ZERO_REPEATED_SERIES,_get_status_code_EMPTY_SERIES \
		  -sEXPORTED_FUNCTIONS=_get_status_code_TIME_OUT_OF_BOUND,_get_status_code_ADDITIVE_OVERFLOW \
		  -sEXPORTED_FUNCTIONS=_get_status_code_NULL_HEADER_SOURCE,_get_status_code_NULL_HEADER_TARGET \
		  -sEXPORTED_FUNCTIONS=_get_status_code_NULL_META_SOURCE,_get_status_code_NULL_META_TARGET \
		  -sEXPORTED_FUNCTIONS=_get_status_code_NULL_SERIES_SOURCE,_get_status_code_NULL_SERIES_TARGET \
		  -sEXPORTED_FUNCTIONS=_get_status_code_NULL_SOURCE,_get_status_code_NULL_TARGET \
		  -sEXPORTED_FUNCTIONS=_get_status_code_NULL_ADDITIVE_SOURCE,_get_status_code_NULL_ADDITIVE_TARGET \
		  -sEXPORTED_FUNCTIONS=_get_status_code_RUNTIME_ERROR,_get_farming_tec_code_REGULAR \
		  -sEXPORTED_FUNCTIONS=_get_farming_tec_code_INDOOR,_get_farming_tec_code_INDOOR_PROTECTED \
		  -sEXPORTED_FUNCTIONS=_get_farming_tec_code_OUTDOOR,_get_farming_tec_code_ARTIFICIAL_SOIL \
		  -sEXPORTED_FUNCTIONS=_get_farming_tec_code_HYDROPONICS,_get_farming_tec_code_ANTHROPONICS \
		  -sEXPORTED_FUNCTIONS=_get_farming_tec_code_AEROPONICS,_get_farming_tec_code_FOGPONICS \
		  -sEXPORTED_FUNCTIONS=_get_reduction_code_NONE,_get_reduction_code_AVG,_get_reduction_code_MAVG
SOURCES = ../../src/adf.c ../../src/crc.c ../../src/lookup_table.c

$(WASM_OUT): $(SOURCES)
	$(CC) $(CARGS) $^ -o $@ $(EMCCARGS) $(EXPORTS)

.PHONY: clean
clean: $(WASM_OUT)
	rm -f $^
