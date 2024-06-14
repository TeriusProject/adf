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

#include "../src/adf.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>

#define FILE_PATH "sample.adf"

int main()
{
	adf_t expected = get_default_object();
	uint8_t *bytes;
	FILE *sample_file = fopen(FILE_PATH, "rb");
	long file_len;
	size_t series_size, h_and_meta_size;

	if (sample_file == NULL) {
		printf("The file `%s` is not opened.", FILE_PATH);
		exit(0);
	}
	fseek(sample_file, 0, SEEK_END);
	file_len = ftell(sample_file);
	rewind(sample_file);

	bytes = (uint8_t *)malloc(file_len * sizeof(uint8_t));
	fread(bytes, file_len, 1, sample_file);
	fclose(sample_file);

	printf("expected bytes: %zu\n", size_adf_t(expected));
	printf("read bytes: %ld\n", file_len);
	assert_true(size_adf_t(expected) == (unsigned long)file_len, "are byte arrays of the same length");

	adf_t new;
	long res = unmarshal(&new, bytes);
	if (res != OK) {
		printf("[%ld] %s", res, "An error occurred during unmarshal process\n");
		return 1;
	}

	/* Header */
	printf("%s\n", "(header - from byte 0)");
	assert_int_equal(new.header.signature, expected.header.signature, "are signatures equals");
	assert_true(new.header.version == expected.header.version, "are versions equals");
	assert_true(new.header.farming_tec == expected.header.farming_tec, "are farming tecniques equals");
	assert_int_equal(new.header.n_wavelength, expected.header.n_wavelength, "are n_wavelengths equal");
	assert_int_equal(new.header.min_w_len_nm, expected.header.min_w_len_nm, "are min_w_len_nms equal");
	assert_int_equal(new.header.max_w_len_nm, expected.header.max_w_len_nm, "are max_w_len_nms equal");
	assert_int_equal(new.header.n_chunks, expected.header.n_chunks, "are n_chunks equal");

	/* Metadata */
	printf("%s\n", "(metadata - from byte 24)");
	assert_int_equal(new.metadata.size_series, expected.metadata.size_series, "are size_series equal");
	assert_int_equal(new.metadata.period_sec, expected.metadata.period_sec, "are periods equal");
	assert_small_int_equal(new.metadata.n_additives, expected.metadata.n_additives, "are number of additive codes equal");
	assert_int_arrays_equal(new.metadata.additive_codes, expected.metadata.additive_codes, new.metadata.n_additives.val, "are additive codes equal");

	/* Series */
	if (new.metadata.size_series.val == 0)
		return 0;

	h_and_meta_size = size_header() + size_medatata_t(new.metadata);
	series_size		= size_series_t(new.header.n_chunks.val, new.series[0]);
	for (uint32_t i = 0; i < new.metadata.size_series.val; i++) {
		printf("(iteration %u - from byte %lu)\n", i, h_and_meta_size + (i * series_size));
		assert_series_equal(new, new.series[i], expected.series[i]);
	}
}
