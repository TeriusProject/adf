/* test_copy.c
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
#include "test.h"
#include "mock.h"

void copy_header_with_null_target(void)
{
	adf_t adf = get_default_object();
	adf_header_t *target = NULL;
	uint16_t res;

	res = cpy_adf_header(target, &adf.header);
	assert_true(res == ADF_NULL_HEADER_TARGET, 
				"header target is null: raised ADF_NULL_HEADER_TARGET");
	
	adf_free(&adf);
}

void copy_header_with_null_source(void)
{
	adf_header_t *source = NULL;
	adf_header_t target;
	uint16_t res;

	res = cpy_adf_header(&target, source);
	assert_true(res == ADF_NULL_HEADER_SOURCE, 
				"header source is null: raised ADF_NULL_HEADER_SOURCE");
}

void headers_are_equal(void)
{
	adf_t adf = get_default_object();
	adf_header_t target;
	uint16_t res;

	res = cpy_adf_header(&target, &adf.header);
	if (res != ADF_OK) {
		printf("[%x] An error occurred while copying the headers\n", res);
		exit(1);
	}
	assert_header_equal_verbose(adf.header, target);

	adf_free(&adf);
}

void copy_meta_with_null_target(void)
{
	adf_t adf = get_default_object();
	adf_meta_t *target = NULL;
	uint16_t res;

	res = cpy_adf_metadata(target, &adf.metadata);
	assert_true(res == ADF_NULL_META_TARGET, 
				"metadata target is null: raised ADF_NULL_META_TARGET");
	
	adf_free(&adf);
}

void copy_meta_with_null_source(void)
{
	adf_meta_t *source = NULL;
	adf_meta_t target;
	uint16_t res;

	res = cpy_adf_metadata(&target, source);
	assert_true(res == ADF_NULL_META_SOURCE, 
				"metadata source is null: raised ADF_NULL_META_SOURCE");
}

void metadata_are_equal(void)
{
	adf_t adf = get_default_object();
	adf_meta_t target;
	uint16_t res;

	res = cpy_adf_metadata(&target, &adf.metadata);
	if (res != ADF_OK) {
		printf("[%x] An error occurred while copying the metadata\n", res);
		exit(1);
	}
	assert_metadata_equal_verbose(adf.metadata, target);

	adf_free(&adf);
	metadata_free(&target);
}

void copied_metadata_arrays_should_have_different_mem_address(void)
{
	adf_t adf = get_default_object();
	adf_meta_t target;
	uint16_t res;

	res = cpy_adf_metadata(&target, &adf.metadata);
	if (res != ADF_OK) {
		printf("[%x] An error occurred while copying the metadata\n", res);
		exit(1);
	}

	assert_true(target.additive_codes != adf.metadata.additive_codes
				|| !target.additive_codes,
				"additive_codes arrays have different memory address");
	
	adf_free(&adf);
	metadata_free(&target);
}

void copy_series_with_null_target(void)
{
	adf_t adf = get_default_object();
	series_t source = adf.series[0];
	series_t *target = NULL;
	uint16_t res;
	
	res = cpy_adf_series(target, &source, &adf);
	assert_true(res == ADF_NULL_SERIES_TARGET, 
				"sereis target is null: raised ADF_NULL_SERIES_TARGET");
	
	adf_free(&adf);
}

void copy_series_with_null_source(void)
{
	adf_t adf = get_default_object();
	series_t *source = NULL;
	series_t target;
	uint16_t res;

	res = cpy_adf_series(&target, source, &adf);
	assert_true(res == ADF_NULL_SERIES_SOURCE, 
				"series source is null: raised ADF_NULL_SERIES_SOURCE");
	
	adf_free(&adf);
}

void series_are_equal(void)
{
	adf_t adf = get_default_object();
	series_t source = adf.series[0];
	series_t target;
	uint16_t res;

	res = cpy_adf_series(&target, &source, &adf);
	if (res != ADF_OK) {
		printf("[%x] %s", res, "An error occurred while copying the series\n");
		exit(1);
	}
	assert_series_equal_verbose(adf, source, target);

	series_free(&target);
	adf_free(&adf);
}

void copied_series_arrays_should_have_different_mem_address(void)
{
	adf_t adf = get_default_object();
	series_t source = adf.series[0];
	series_t target;
	uint16_t res;

	res = cpy_adf_series(&target, &source, &adf);
	if (res != ADF_OK) {
		printf("[%x] %s", res, "An error occurred while copying the series\n");
		exit(1);
	}

	assert_true(target.env_temp_c != source.env_temp_c
				|| !target.env_temp_c,
				"env_temp_c arrays have different memory address");
	assert_true(target.water_use_ml != source.water_use_ml
				|| !target.water_use_ml,
				"water_use_ml arrays have different memory address");
	assert_true(target.light_exposure != source.light_exposure
				|| !target.light_exposure,
				"light_exposure arrays have different memory address");
	assert_true(target.soil_additives != source.soil_additives
				|| !target.soil_additives,
				"soil_additives arrays have different memory address");
	assert_true(target.atm_additives != source.atm_additives
				|| !target.atm_additives,
				"atm_additives arrays have different memory address");
	
	adf_free(&adf);
	series_free(&target);
}

void copy_adf_with_null_target(void)
{
	adf_t adf = get_default_object();
	adf_t *target = NULL;
	uint16_t res;

	res = cpy_adf(target, &adf);
	assert_true(res == ADF_NULL_TARGET, 
				"adf target is null: raised ADF_NULL_TARGET");
	
	adf_free(&adf);
}

void copy_adf_with_null_source(void)
{
	adf_t *source = NULL;
	adf_t target;
	uint16_t res;

	res = cpy_adf(&target, source);
	assert_true(res == ADF_NULL_SOURCE, 
				"adf source is null: raised ADF_NULL_SOURCE");
}

void adfs_are_equal(void)
{
	adf_t source = get_default_object();
	adf_t target;
	uint16_t res;

	res = cpy_adf(&target, &source);
	if (res != ADF_OK) {
		printf("[%x] An error occurred while copying the adf files\n", res);
		exit(1);
	}
	printf("%s\n", "----------------------  ADF  ----------------------");
	/* Header */
	printf("%s\n", "(header)");
	assert_header_equal_verbose(target.header, source.header);

	/* Metadata */
	printf("%s\n", "(metadata)");
	assert_metadata_equal_verbose(target.metadata, source.metadata);

	/* Series */
	if (source.metadata.size_series.val == 0) return;

	for (uint32_t i = 0; i < source.metadata.size_series.val; i++) {
		printf("(iteration %u)\n", i);
		assert_series_equal_verbose(source, target.series[i], source.series[i]);
	}
	printf("%s\n", "----------------------  END  ----------------------");

	adf_free(&source);
	adf_free(&target);
}

int main(void)
{
	/* Header */
	copy_header_with_null_target();
	copy_header_with_null_source();
	headers_are_equal();

	/* Metadata */
	copy_meta_with_null_target();
	copy_meta_with_null_source();
	metadata_are_equal();
	copied_metadata_arrays_should_have_different_mem_address();

	/* Series */
	copy_series_with_null_target();
	copy_series_with_null_source();
	series_are_equal();
	copied_series_arrays_should_have_different_mem_address();

	/* ADF */
	copy_adf_with_null_target();
	copy_adf_with_null_source();
	adfs_are_equal();
}
