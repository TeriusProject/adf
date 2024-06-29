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
#include "../src/adf.h"
#include <stdio.h>
#include <stdlib.h>



void test_update_series_time_out_of_bound(void)
{
	adf_t adf = get_default_object();
	uint16_t res;
	uint64_t time = adf.metadata.n_series * adf.metadata.period_sec.val;
	res = update_series(&adf, adf.series[0], time+1);
	assert_true(res == ADF_TIME_OUT_OF_BOUND,
				"Time out of bound: should return ADF_TIME_OUT_OF_BOUND");
}

void test_update_series(void)
{
	adf_t adf = get_default_object();
	series_t to_add = get_repeated_series();
	uint16_t res;
	uint64_t time = 1;
	res = update_series(&adf, to_add, time);
	assert_true(res == ADF_OK, "Series updated");
	assert_series_equal(adf, adf.series[0], to_add, "Series are equal");
	assert_true(adf.metadata.n_series == 5, "There should be 5 series in adf");
}

int main(void)
{
	test_update_series_time_out_of_bound();
	test_update_series();
}
