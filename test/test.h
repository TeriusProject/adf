/*
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
 * Teriusis distributed in the hope that it will be useful,
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
#include <stdbool.h>
#include <stdio.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define EPSILON 1e-6

void assert_true(bool condition, const char *label);
bool are_ints_equal(uint_t x, uint_t y);
bool are_small_ints_equal(uint_small_t x, uint_small_t y);
bool are_reals_equal(real_t x, real_t y);
bool are_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size);
bool are_int_arrays_equal(uint_t *x, uint_t *y, uint32_t size);
bool are_real_arrays_equal(real_t *x, real_t *y, size_t size);
bool are_additive_arrays_equal(additive_t *x, additive_t *y, size_t size);
void assert_int_equal(uint_t x, uint_t y, const char *label);
void assert_long_equal(long x, long y, const char *label);
void assert_real_equal(real_t x, real_t y, const char *label);
void assert_small_int_equal(uint_small_t x, uint_small_t y, const char *label);
void assert_int_arrays_equal(uint_t *x, uint_t *y, uint32_t size, const char *label);
void assert_real_arrays_equal(real_t *x, real_t *y, uint32_t size, const char *label);
void assert_additive_arrays_equal(additive_t *x, additive_t *y, uint32_t size, const char *label);
void assert_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size, const char *label);
void assert_series_equal(adf_t data, series_t x, series_t y);

adf_t get_default_object(void);

#endif /* __UTILS_H__ */
