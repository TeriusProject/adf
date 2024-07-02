/*
 * test_series_remove.c
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

#include "mock.h"
#include "test.h"
#include "../src/adf.h"
#include <stdio.h>
#include <stdlib.h>

void test_remove_repeated_series(void)
{
	adf_t adf = get_default_object();
	int res = remove_series(&adf);
	if (res != ADF_OK) { printf("Error during remove. Error code [%u]", res); }

	uint_t expected_series_size = { 2 };
	assert_int_equal(adf.metadata.size_series, expected_series_size,
					 "The size of series array is 2");
	size_t expected_n_series = 3;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 3 series");
}

void test_remove(void)
{
	adf_t adf = get_default_object();
	int res = remove_series(&adf);
	if (res != ADF_OK) { printf("Error during remove. Error code [%u]", res); }

	uint_t expected_size_series = { 2 };
	assert_int_equal(adf.metadata.size_series, expected_size_series,
					 "The size of series array is 2");

	size_t expected_n_series = 3;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 3 series");
}

void test_remove_from_empty_series(void)
{
	adf_t adf = get_object_with_zero_series();
	int res = remove_series(&adf);
	assert_true(res == ADF_EMPTY_SERIES,
				"Cannot remove anything from EMPTY series");
}

void test_remove_last_series(void)
{
	adf_t adf = get_default_object();
	int res;

	for (uint8_t i = 0; i < 4; i++) {
		res = remove_series(&adf);
		if (res != ADF_OK) {
			printf("Error during remove. Error code [%u]", res);
		}
	}

	uint_t expected_size_series = { 0 };
	assert_int_equal(adf.metadata.size_series, expected_size_series,
					 "The series array is EMPTY");

	size_t expected_n_series = 0;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 0 series");
}

int main(void)
{
	test_remove_repeated_series();
	test_remove();
	test_remove_from_empty_series();
	test_remove_last_series();
}
