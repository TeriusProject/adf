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
EXPORTS = -sEXPORTED_FUNCTIONS=_malloc,_free,_marshal,_unmarshal,_add_series,_remove_series,_get_hex_version,_size_header,_size_adf_t,_create_header,_create_additive,_create_wavelength_info,_create_reduction_info,_create_soil_depth_info,_create_trans_soil_depth_info,_create_precision_info,_get_status_code_OK,_get_status_code_HEADER_CORRUPTED,_get_status_code_METADATA_CORRUPTED,_get_status_code_SERIES_CORRUPTED,_get_status_code_ZERO_REPEATED_SERIES,_get_status_code_EMPTY_SERIES,_get_status_code_TIME_OUT_OF_BOUND,_get_status_code_ADDITIVE_OVERFLOW,_get_status_code_NULL_HEADER_SOURCE,_get_status_code_NULL_HEADER_TARGET,_get_status_code_NULL_META_SOURCE,_get_status_code_NULL_META_TARGET,_get_status_code_NULL_SERIES_SOURCE,_get_status_code_NULL_SERIES_TARGET,_get_status_code_NULL_SOURCE,_get_status_code_NULL_TARGET,_get_status_code_NULL_ADDITIVE_SOURCE,_get_status_code_NULL_ADDITIVE_TARGET,_get_status_code_RUNTIME_ERROR,_get_farming_tec_code_REGULAR,_get_farming_tec_code_INDOOR,_get_farming_tec_code_INDOOR_PROTECTED,_get_farming_tec_code_OUTDOOR,_get_farming_tec_code_ARTIFICIAL_SOIL,_get_farming_tec_code_HYDROPONICS,_get_farming_tec_code_ANTHROPONICS,_get_farming_tec_code_AEROPONICS,_get_farming_tec_code_FOGPONICS,_get_reduction_code_NONE,_get_reduction_code_AVG,_get_reduction_code_MAVG,_get_UINT_BIG_T_SIZE,_get_UINT_T_SIZE,_get_UINT_SMALL_T_SIZE,_get_UINT_TINY_T_SIZE,_get_REAL_T_SIZE,_get_ADD_T_SIZE,_get_ADF_DAY,_get_ADF_WEEK,_get_ADF_MONTH_28,_get_ADF_MONTH_29,_get_ADF_MONTH_30,_get_ADF_MONTH_31
SOURCES = ../../src/adf.c ../../src/crc.c ../../src/lookup_table.c

$(WASM_OUT): $(SOURCES)
	$(CC) $(CARGS) $^ -o $@ $(EMCCARGS) $(EXPORTS)

.PHONY: clean
clean: $(WASM_OUT)
	rm -f $^
