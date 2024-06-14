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
 * Teriusis distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "../src/adf.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>

void test_add_series(void)
{
	adf_t adf = get_default_object();
	series_t series = get_series();
	int res = add_series(&adf, series);
	if (res != OK) {
		printf("Error during update. Error code [%u]", res);
	}
	uint_t expected_n_series = {3};
	assert_int_equal(
		adf.metadata.n_series,
		expected_n_series,
		"There are 3 series"
	);
}

void test_add_repeated_series(void)
{
	adf_t adf = get_default_object();
	series_t series = get_repeated_series();
	int res = add_series(&adf, series);
	if (res != OK) {
		printf("Error during update. Error code [%u]", res);
	}
	uint_t expected_n_series = {2};
	assert_int_equal(
		adf.metadata.n_series,
		expected_n_series,
		"There are 2 series"
	);

	uint_t expected_repeated_last_series = {5};
	series_t last = adf.series[adf.metadata.n_series.val - 1];
	assert_int_equal(
		last.repeated,
		expected_repeated_last_series,
		"The last one is repeated 5 times"
	);
}

void test_add_repeated_and_non_repeated_series(void)
{
	adf_t adf = get_default_object();
	series_t series1 = get_series();
	series_t series2 = get_repeated_series();
	int res = add_series(&adf, series1);
	if (res != OK) {
		printf("Error during update. Error code [%u]", res);
	}
	res = add_series(&adf, series2);
	if (res != OK) {
		printf("Error during update. Error code [%u]", res);
	}
	uint_t expected_n_series = {4};
	assert_int_equal(
		adf.metadata.n_series,
		expected_n_series,
		"There are 4 series"
	);

	uint_t expected_repeated_last_series = {2};
	series_t last = adf.series[adf.metadata.n_series.val - 1];
	assert_int_equal(
		last.repeated,
		expected_repeated_last_series,
		"The last one is repeated 2 times"
	);
}

int main()
{
	test_add_series();
	test_add_repeated_series();
	test_add_repeated_and_non_repeated_series();
}
