/*
 * test_series_update.c
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

void test_update_series_time_out_of_bound(void)
{
	adf_t adf;
	uint16_t res;
	uint64_t time;

	adf = get_default_object();
	time = adf.metadata.n_series * adf.metadata.period_sec.val;
	res = update_series(&adf, adf.series, time+1);

	assert_true(res == ADF_TIME_OUT_OF_BOUND,
				"Time out of bound: should return ADF_TIME_OUT_OF_BOUND");
	
	adf_free(&adf);
}

void test_update_one_series(void)
{
	adf_t adf;
	series_t to_update;
	uint16_t res;
	uint64_t time = 1;

	adf = get_default_object();
	to_update = get_repeated_series();
	res = update_series(&adf, &to_update, time);

	assert_true(res == ADF_OK, "Series updated");
	assert_series_equal(adf, adf.series[0], to_update, "Series are equal");
	assert_true(adf.metadata.n_series == 5, "There should be 5 series in adf");

	series_free(&to_update);
	adf_free(&adf);
}

void update_one_series_with_an_equal_one(void)
{
	adf_t adf;
	series_t to_update;
	uint16_t res;
	uint64_t time = 1;

	adf = get_default_object();
	res = cpy_adf_series(&to_update, adf.series, &adf);
	if (res != ADF_OK) {
		printf("An error occurred [code:%x]\n", res);
		exit(1);
	}
	res = update_series(&adf, &to_update, time);
	if (res != ADF_OK) {
		printf("An error occurred [code:%x]\n", res);
		exit(1);
	}

	assert_true(are_series_equal(&to_update, adf.series, &adf),
				"Update the same series, series should be equal");
	assert_int_equal(to_update.repeated, adf.series[0].repeated,
					 "Repeated field should be updated");

	series_free(&to_update);
	adf_free(&adf);
}

void update_one_series_with_an_equal_one_with_different_repetition(void)
{
	adf_t adf;
	series_t to_update;
	uint16_t res;
	uint64_t time = 1;
	uint_t expected_repeated = { 3 };

	adf = get_default_object();
	res = cpy_adf_series(&to_update, adf.series, &adf);
	if (res != ADF_OK) {
		printf("An error occurred [code:%x]\n", res);
		exit(1);
	}
	/* update the repeated field */
	to_update.repeated.val = 3;
	res = update_series(&adf, &to_update, time);
	if (res != ADF_OK) {
		printf("An error occurred [code:%x]\n", res);
		exit(1);
	}

	assert_true(are_series_equal(&to_update, adf.series, &adf),
				"Update the same series (with different repetition), "
				"series should be equal");
	assert_int_equal(adf.series->repeated, expected_repeated,
					 "Repeated field should be updated");

	series_free(&to_update);
	adf_free(&adf);
}

void test_update_series_within_repeated_series(void)
{
	adf_t adf;
	series_t series1, to_update;
	uint16_t res;
	uint64_t time;

	adf = get_object_with_zero_series();
	to_update = get_series();
	series1 = get_series_with_two_soil_additives();
	series1.repeated.val = 3;
	res = add_series(&adf, &series1);
	if (res != ADF_OK) {
		printf("An error occurred while updating [code:%x]\n", res);
		exit(1);
	}
	time = adf.metadata.period_sec.val + 1;
	res = update_series(&adf, &to_update, time);
	if (res != ADF_OK) {
		printf("An error occurred while updating [code:%x]\n", res);
		exit(1);
	}

	assert_true(adf.metadata.size_series.val == 3,
				"size_series should be 4");
	assert_true(are_series_equal(adf.series, &series1, &adf),
				"adf.series[0] == series1");
	assert_true(are_series_equal(adf.series + 1, &to_update, &adf),
				"adf.series[1] == to_update");
	assert_true(are_series_equal(adf.series + 2, &series1, &adf),
				"adf.series[2] == series1");
	assert_true(adf.series[0].repeated.val == 1,
				"First series should be repeated 1 time");
	assert_true(adf.series[1].repeated.val == 1,
				"Second series should be repeated 1 time");
	assert_true(adf.series[2].repeated.val == 1,
				"Third series should be repeated 1 time");

	series_free(&to_update);
	series_free(&series1);
	adf_free(&adf);
}

void test_update_series_within_repeated_series_between_others(void)
{
	adf_t adf;
	series_t series1, series2, to_update;
	uint16_t res;
	uint64_t time;

	adf = get_object_with_zero_series();
	to_update = get_series();
	series1 = get_series_with_two_soil_additives();
	series1.repeated.val = 3;
	res = add_series(&adf, &series1);
	if (res != ADF_OK) {
		printf("An error occurred while updating [code:%x]\n", res);
		exit(1);
	}
	series2 = get_repeated_series();
	res = add_series(&adf, &series2);
	if (res != ADF_OK) {
		printf("An error occurred while updating [code:%x]\n", res);
		exit(1);
	}
	res = add_series(&adf, &series1);
	if (res != ADF_OK) {
		printf("An error occurred while updating [code:%x]\n", res);
		exit(1);
	}
	time = (adf.metadata.period_sec.val * 4) + 1;
	res = update_series(&adf, &to_update, time);
	if (res != ADF_OK) {
		printf("An error occurred while updating [code:%x]\n", res);
		exit(1);
	}

	assert_true(adf.metadata.size_series.val == 4,
				"size_series should be 4");
	assert_true(are_series_equal(adf.series, &series1, &adf),
				"adf.series[0] == series1");
	assert_true(are_series_equal(adf.series + 1, &series2, &adf),
				"adf.series[1] == series2");
	assert_true(are_series_equal(adf.series + 2, &to_update, &adf),
				"adf.series[2] == to_update");
}

int main(void)
{
	test_update_series_time_out_of_bound();
	test_update_one_series();
	update_one_series_with_an_equal_one();
	update_one_series_with_an_equal_one_with_different_repetition();
	test_update_series_within_repeated_series();
	test_update_series_within_repeated_series_between_others();
}
