/*
 * test_series_add.c
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

void test_add_series(void)
{
	adf_t adf = get_default_object();
	series_t series = get_series();
	int res = add_series(&adf, &series);
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
	int res = add_series(&adf, &series);
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
	int res = add_series(&adf, &series1);
	if (res != ADF_OK) { printf("Error during update. Error code [%u]", res); }
	res = add_series(&adf, &series2);
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
	int res = add_series(&adf, &series);
	if (res != ADF_OK) { printf("Error during update. Code [%u]", res); }
	uint_t expected_series_size = { 1 };
	assert_int_equal(adf.metadata.size_series, expected_series_size,
					 "The size of series array is 1");
	
	size_t expected_n_series = 1;
	assert_true(adf.metadata.n_series == expected_n_series,
				"There are 1 series");
}

void test_add_series_should_merge_additives(void)
{
	int res;
	adf_t adf = get_object_with_zero_series();
	series_t series1 = get_series();
	series_t series2 = get_series();
	res = add_series(&adf, &series1);
	if (res != ADF_OK) { printf("Error during update. Code [%u]", res); }

	assert_true(adf.metadata.n_additives.val == 1,
				"metadata should contain 1 soil additive");
	assert_true(adf.series[0].soil_additives[0].code_idx.val == 0,
				"additives should occupy the lowest possible position in metadata");
	series2.soil_additives[0].code.val = 5678;
	series2.soil_additives[0].concentration.val = 5.678;
	res = add_series(&adf, &series2);
	if (res != ADF_OK) { printf("Error during update. Code [%u]", res); }
	
	assert_true(adf.metadata.n_additives.val == 2,
				"metadata should contain 2 soil additive");
	assert_true(adf.series[0].soil_additives[0].code_idx.val == 0,
				"add_series should never change already inserted additives");
	assert_true(adf.series[1].soil_additives[0].code_idx.val == 1,
				"additives should be inserted by push_back");
}

void test_additive_overflow(void)
{
	adf_t adf = get_object_with_zero_series();
	series_t series1, series2;
	additive_t *series1_additives, *series2_additives;
	uint16_t size1 = 30'000, size2 = 50'000, res;

	series1_additives = malloc(size1 * sizeof(additive_t));
	series2_additives = malloc(size2 * sizeof(additive_t));

	for (uint16_t i = 0; i < size1; i++)
		series1_additives[i] = (additive_t) {
			.code = { i },
			.concentration = { 1.0 }
		};

	for (uint16_t i = 0; i < size2; i++)
		series1_additives[i] = (additive_t) {
			.code = { i + size1 },
			.concentration = { 1.0 }
		};
	series1 = (series_t) { 
		.light_exposure = get_real_array(),
		.temp_celsius = get_real_array(),
		.water_use_ml = get_real_array(),
		.pH = 11,
		.p_bar = { 13.56789 },
		.soil_density_kg_m3 = { 123.345 },
		.n_soil_adds = { size1 },
		.n_atm_adds = { 0 },
		.soil_additives = series1_additives,
		.atm_additives = NULL,
		.repeated = { 1 }
	};
	series2 = (series_t) { 
		.light_exposure = get_real_array(),
		.temp_celsius = get_real_array(),
		.water_use_ml = get_real_array(),
		.pH = 2,
		.p_bar = { 3.89 },
		.soil_density_kg_m3 = { 0.345 },
		.n_soil_adds = { size2 },
		.n_atm_adds = { 0 },
		.soil_additives = series2_additives,
		.atm_additives = NULL,
		.repeated = { 1 }
	};
	res = add_series(&adf, &series1);
	if (res != ADF_OK) {
		printf("An error occurred while adding series 1 [code:%x]", res);
		exit(1);
	}
	res = add_series(&adf, &series2);
	assert_true(res == ADF_ADDITIVE_OVERFLOW, 
				"too many additives, should return ADF_ADDITIVE_OVERFLOW");
}

int main(void)
{
	test_add_series();
	test_add_repeated_series();
	test_add_repeated_and_non_repeated_series();
	test_add_to_empty_series();
	test_add_series_should_merge_additives();
	test_additive_overflow();
}