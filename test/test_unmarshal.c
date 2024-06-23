/* test_unmarshal.c
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
#include "mock.h"
#include "test.h"
#include <stdio.h>
#include <stdlib.h>

#define FILE_PATH "sample.adf"

int main(void)
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
	assert_true(
		size_adf_t(expected) == (unsigned long)file_len,
		"are byte arrays of the same length"
	);

	adf_t new;
	long res = unmarshal(&new, bytes);
	if (res != ADF_OK) {
		printf("[%ld] %s", res, "An error occurred during unmarshal process\n");
		return 1;
	}

	/* Header */
	printf("%s\n", "(header - from byte 0)");
	assert_header_equal(new.header, expected.header);

	/* Metadata */
	printf("%s\n", "(metadata - from byte 24)");
	assert_metadata_equal(new.metadata, expected.metadata);

	/* Series */
	if (new.metadata.size_series.val == 0) return 0;

	h_and_meta_size = size_header() + size_medatata_t(new.metadata);
	series_size = size_series_t(new.header.n_chunks.val, new.series[0]);
	for (uint32_t i = 0; i < new.metadata.size_series.val; i++) {
		printf(
			"(iteration %u - from byte %lu)\n", i,
			h_and_meta_size + (i * series_size)
		);
		assert_series_equal(new, new.series[i], expected.series[i]);
	}
}
