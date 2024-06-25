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
 * Terius is distributed in the hope that it will be useful,
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
	assert_header_equal(adf.header, target);
}

void copy_meta_with_null_target(void)
{
	adf_t adf = get_default_object();
	adf_meta_t *target = NULL;
	uint16_t res;
	res = cpy_adf_metadata(target, &adf.metadata);
	assert_true(res == ADF_NULL_META_TARGET, 
				"metadata target is null: raised ADF_NULL_META_TARGET");
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
	assert_metadata_equal(adf.metadata, target);
}

void copy_series_with_null_target(void)
{
	adf_t adf = get_default_object();
	series_t source = adf.series[0];
	series_t *target = NULL;
	uint16_t res;
	res = cpy_adf_series(target, &source, adf.header.n_chunks.val,
						 adf.header.n_wavelength.val);
	assert_true(res == ADF_NULL_SERIES_TARGET, 
				"sereis target is null: raised ADF_NULL_SERIES_TARGET");
}

void copy_series_with_null_source(void)
{
	adf_t adf = get_default_object();
	series_t *source = NULL;
	series_t target;
	uint16_t res;
	res = cpy_adf_series(&target, source, adf.header.n_chunks.val,
						 adf.header.n_wavelength.val);
	assert_true(res == ADF_NULL_SERIES_SOURCE, 
				"series source is null: raised ADF_NULL_SERIES_SOURCE");
}

void series_are_equal(void)
{
	adf_t adf = get_default_object();
	series_t source = adf.series[0];
	series_t target;
	uint16_t res;
	res = cpy_adf_series(&target, &source, adf.header.n_chunks.val,
						 adf.header.n_wavelength.val);
	if (res != ADF_OK) {
		printf("[%x] %s", res, "An error occurred while copying the series\n");
		exit(1);
	}

	assert_series_equal_verbose(adf, source, target);
}

void copy_adf_with_null_target(void)
{
	adf_t adf = get_default_object();
	adf_t *target = NULL;
	uint16_t res;
	res = cpy_adf(target, &adf);
	assert_true(res == ADF_NULL_TARGET, 
				"adf target is null: raised ADF_NULL_TARGET");
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
	/* Header */
	printf("%s\n", "(header)");
	assert_header_equal(target.header, source.header);

	/* Metadata */
	printf("%s\n", "(metadata)");
	assert_metadata_equal(target.metadata, source.metadata);

	/* Series */
	if (source.metadata.size_series.val == 0) return;

	for (uint32_t i = 0; i < source.metadata.size_series.val; i++) {
		printf("(iteration %u)\n", i);
		assert_series_equal_verbose(source, target.series[i], source.series[i]);
	}
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

	/* Series */
	copy_series_with_null_target();
	copy_series_with_null_source();
	series_are_equal();

	/* ADF */
	copy_adf_with_null_target();
	copy_adf_with_null_source();
	adfs_are_equal();
}