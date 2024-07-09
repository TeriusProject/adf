/* test.h - A minimal unit test framework interface
 * ------------------------------------------------------------------------
 * ADF - Agriculture Data Format
 * Copyright (C) 2024 Matteo Nicoli
 *
 * This file is part of Terius
 *
 * ADF is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ADF is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include "../src/adf.h"
#include "../src/lookup_table.h"
#include <stdbool.h>
#include <stdio.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define TICK "\u2713"
#define CROSS "\u2A09"

#define WARN_SAMPLE  "WARNING: for this test to work properly, the file "     \
					 "sample.adf must be up to date to the latest version. "  \
					 "If not, run `make init` first."

bool are_ints_equal(uint_t, uint_t);
bool are_small_ints_equal(uint_small_t, uint_small_t);
bool are_reals_equal(real_t, real_t);
bool are_uint8_arrays_equal(uint8_t *, uint8_t *, uint32_t);
bool are_int_arrays_equal(uint_t *, uint_t *, uint32_t);
bool are_real_arrays_equal(real_t *, real_t *, size_t);
bool are_additive_arrays_equal(additive_t *, additive_t *, size_t);
bool are_pair_arrays_equal(pair_t *, pair_t *, uint32_t);
bool are_additives_equal(additive_t, additive_t);

/* assertions */
void assert_true(bool, const char *);
void assert_int_equal(uint_t, uint_t, const char *);
void assert_long_equal(long, long, const char *);
void assert_real_equal(real_t, real_t, const char *);
void assert_small_int_equal(uint_small_t, uint_small_t, const char *);
void assert_uint32_arrays_equal(uint32_t *, uint32_t *, uint32_t, 
								const char *);
void assert_int_arrays_equal(uint_t *, uint_t *, uint32_t, const char *);
void assert_real_arrays_equal(real_t *, real_t *, uint32_t, const char *);
void assert_additive_arrays_equal(additive_t *, additive_t *, uint32_t,
								  const char *);
void assert_uint8_arrays_equal(uint8_t *, uint8_t *, uint32_t, const char *);
void assert_pair_arrays_equal(pair_t *, pair_t *, uint32_t, const char *);
void assert_header_equal(adf_header_t, adf_header_t, const char *);
void assert_header_equal_verbose(adf_header_t, adf_header_t);
void assert_metadata_equal_verbose(adf_meta_t, adf_meta_t);
void assert_metadata_equal(adf_meta_t, adf_meta_t, const char *);
void assert_series_equal_verbose(adf_t, series_t, series_t);
void assert_series_equal(adf_t, series_t, series_t, const char *);

/* Performances */
typedef struct timespec tick_t;
uint64_t get_nanos(void);
double get_time_diff(uint64_t);

#endif /* __UTILS_H__ */
