/* test_create.c
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

#include "../src/adf.h"
#include "mock.h"
#include "test.h"

void test_init_adf(void)
{
	adf_t adf;
	adf_header_t header = get_default_header();
	adf_init(&adf, header, 3600);
	assert_header_equal(adf.header, header, "headers should be equal");
	assert_true(!adf.series, "series should be initialized to NULL");
	assert_true(!adf.metadata.additive_codes,
				"additive_codes should be initialized to NULL");
	adf_free(&adf);
}

void add_seed_time_out_of_bound(void)
{
	adf_t adf;
	adf_header_t header = get_default_header();
	series_t s;
	uint16_t res;

	adf_init(&adf, header, ADF_DAY);
	res = set_seed_time(&adf, 1);
	assert_true(res == ADF_TIME_OUT_OF_BOUND,
				"cannot set a seed time on empty adf");
	s = get_series_with_two_soil_additives();
	res = add_series(&adf, &s);
	if (res != ADF_OK) {
		printf("[%x] An error occurred while adding the series\n", res);
		exit(1);
	}
	res = set_seed_time(&adf, (ADF_DAY * s.repeated.val) + 1);
	assert_true(res == ADF_TIME_OUT_OF_BOUND,
				"seed time cannot exceed the total time length");
	res = set_seed_time(&adf, 1);
	assert_true(res == ADF_OK,
				"correctly set the seed time within the time length");

	adf_free(&adf);
	series_free(&s);
}

void add_harvest_time_out_of_bound(void)
{
	adf_t adf;
	adf_header_t header = get_default_header();
	series_t s;
	uint16_t res;

	adf_init(&adf, header, ADF_DAY);
	res = set_harvest_time(&adf, 1);
	assert_true(res == ADF_TIME_OUT_OF_BOUND,
				"cannot set an harvest time on empty adf");
	s = get_series_with_two_soil_additives();
	res = add_series(&adf, &s);
	if (res != ADF_OK) {
		printf("[%x] An error occurred while adding the series\n", res);
		exit(1);
	}
	res = set_harvest_time(&adf, (ADF_DAY * s.repeated.val) + 1);
	assert_true(res == ADF_TIME_OUT_OF_BOUND,
				"harvest time cannot exceed the total time length");
	res = set_harvest_time(&adf, 1);
	assert_true(res == ADF_OK,
				"correctly set the harvest time within the time length");

	adf_free(&adf);
	series_free(&s);
}

void test_create_default_precision_info(void)
{
	precision_info_t p;
	real_t zero = { 0.0 };

	p = default_precision_info();
	assert_real_equal(p.soil_density_prec, zero,
					  "`soil_density_prec` ==  0 in def. precision_info_t");
	assert_real_equal(p.pressure_prec, zero,
					  "`pressure_prec` ==  0 in def. precision_info_t");
	assert_real_equal(p.light_exposure_prec, zero,
					  "`light_exposure_prec` ==  0 in def. precision_info_t");
	assert_real_equal(p.water_use_prec, zero,
					  "`water_use_prec` ==  0 in def. precision_info_t");
	assert_real_equal(p.soil_temp_prec, zero,
					  "`soil_temp_prec` ==  0 in def. precision_info_t");
	assert_real_equal(p.env_temp_prec, zero,
					  "`env_temp_prec` ==  0 in def. precision_info_t");
}

void test_create_default_reduction_info(void)
{
	reduction_info_t r;

	r = default_reduction_info();
	assert_true(r.soil_density_red_mode == ADF_RM_NONE,
				"`soil_density_red_mode` == NONE in def. reduction_info_t");
	assert_true(r.pressure_red_mode == ADF_RM_NONE,
				"`pressure_red_mode` == NONE in def. reduction_info_t");
	assert_true(r.light_exposure_red_mode == ADF_RM_NONE,
				"`light_exposure_red_mode` == NONE in def. reduction_info_t");
	assert_true(r.water_use_red_mode == ADF_RM_NONE,
				"`.water_use_red_mode` == NONE in def. reduction_info_t");
	assert_true(r.soil_temp_red_mode == ADF_RM_NONE,
				"`soil_temp_red_mode` == NONE in def. reduction_info_t");
	assert_true(r.env_temp_red_mode == ADF_RM_NONE,
				"`env_temp_red_mode` == NONE in def. reduction_info_t");
	assert_true(r.additive_red_mode == ADF_RM_NONE,
				"`additive_red_mode` == NONE in def. reduction_info_t");
}

int main(void)
{
	test_init_adf();
	test_create_default_precision_info();
	test_create_default_reduction_info();
	add_seed_time_out_of_bound();
	add_harvest_time_out_of_bound();
}
