/*
 * test_update.c
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
 * Terius is distributed in the hope that it will be useful,
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
#include <adf.h>
#include <stdio.h>
#include <stdlib.h>

void test_add_series(void)
{
	adf_t adf = get_default_object();
	series_t series = get_series();
	int res = add_series(&adf, series);
	if (res != ADF_OK) { printf("Error during update. Error code [%u]", res); }
	uint_t expected_series_size = { 3 };
	assert_int_equal(adf.metadata.size_series, expected_series_size,
					 "The size of series array is 3");

	size_t expected_n_series = 5;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 5 series");
}

void test_add_repeated_series(void)
{
	adf_t adf = get_default_object();
	series_t series = get_repeated_series();
	int res = add_series(&adf, series);
	if (res != ADF_OK) { printf("Error during update. Error code [%u]", res); }
	uint_t expected_series_size = { 2 };
	assert_int_equal(adf.metadata.size_series, expected_series_size,
					 "The size of series array is 2");

	uint_t expected_repeated_last_series = { 5 };
	series_t last = adf.series[adf.metadata.size_series.val - 1];
	assert_int_equal(last.repeated, expected_repeated_last_series,
					 "The last one is repeated 5 times");

	size_t expected_n_series = 6;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 6 series");
}

void test_add_repeated_and_non_repeated_series(void)
{
	adf_t adf = get_default_object();
	series_t series1 = get_series();
	series_t series2 = get_repeated_series();
	int res = add_series(&adf, series1);
	if (res != ADF_OK) { printf("Error during update. Error code [%u]", res); }
	res = add_series(&adf, series2);
	if (res != ADF_OK) { printf("Error during update. Error code [%u]", res); }

	uint_t expected_series_size = { 4 };
	assert_int_equal(adf.metadata.size_series, expected_series_size,
					 "The size of series array is 4");

	uint_t expected_repeated_last_series = { 2 };
	series_t last = adf.series[adf.metadata.size_series.val - 1];
	assert_int_equal(last.repeated, expected_repeated_last_series,
					 "The last one is repeated 2 times");

	size_t expected_n_series = 7;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 7 series");
}

void test_add_to_empty_series(void)
{
	adf_t adf = get_object_with_zero_series();
	series_t series = get_series();
	int res = add_series(&adf, series);
	if (res != ADF_OK) { printf("Error during update. Error code [%u]", res); }
	uint_t expected_series_size = { 1 };
	assert_int_equal(adf.metadata.size_series, expected_series_size,
					 "The size of series array is 1");

	size_t expected_n_series = 1;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 1 series");
}

void test_delete_repeated_series(void)
{
	adf_t adf = get_default_object();
	int res = remove_series(&adf);
	if (res != ADF_OK) { printf("Error during delete. Error code [%u]", res); }

	uint_t expected_series_size = { 2 };
	assert_int_equal(adf.metadata.size_series, expected_series_size,
					 "The size of series array is 2");
	size_t expected_n_series = 3;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 3 series");
}

void test_delete(void)
{
	adf_t adf = get_default_object();
	int res = remove_series(&adf);
	if (res != ADF_OK) { printf("Error during delete. Error code [%u]", res); }

	uint_t expected_size_series = { 2 };
	assert_int_equal(adf.metadata.size_series, expected_size_series,
					 "The size of series array is 2");

	size_t expected_n_series = 3;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 3 series");
}

void test_delete_from_empty_series(void)
{
	adf_t adf = get_object_with_zero_series();
	int res = remove_series(&adf);
	assert_true(res == ADF_EMPTY_SERIES,
				"Cannot delete anything from EMPTY series");
}

void test_delete_last_series(void)
{
	adf_t adf = get_default_object();
	int res;

	for (uint8_t i = 0; i < 4; i++) {
		res = remove_series(&adf);
		if (res != ADF_OK) {
			printf("Error during delete. Error code [%u]", res);
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
	test_add_series();
	test_add_repeated_series();
	test_add_repeated_and_non_repeated_series();
	test_add_to_empty_series();
	test_delete_repeated_series();
	test_delete();
	test_delete_from_empty_series();
	test_delete_last_series();
}
