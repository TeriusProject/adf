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
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define FILE_PATH "sample.adf"

int main()
{
	adf_t expected = get_default_object();
	uint8_t *bytes;
	FILE *sample_file = fopen(FILE_PATH, "rb");
	long file_len;

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
	
	assert_true(size_adf_t(expected) == (unsigned long)file_len, "are byte arrays of the same length");

	printf("expected bytes: %zu\n", size_adf_t(expected));
	printf("read bytes: %ld\n", file_len);

	adf_t new;
	long res = unmarshal(&new, bytes);
	if (res != OK) {
		printf("%s", "An error occurred during unmarshal process\n");
		return 1;
	}

	assert_int_equal(new.signature, expected.signature, "are signatures equals");
	assert_true(new.version == expected.version, "are versions equals");
	assert_int_equal(new.n_wavelength, expected.n_wavelength, "are n_wavelengths equal");
	assert_int_equal(new.min_w_len_nm, expected.min_w_len_nm, "are min_w_len_nms equal");
	assert_int_equal(new.max_w_len_nm, expected.max_w_len_nm, "are max_w_len_nms equal");
	assert_int_equal(new.period, expected.period, "are periods equal");
	assert_int_equal(new.n_chunks, expected.n_chunks, "are n_chunks equal");
	assert_int_equal(new.n_iterations, expected.n_iterations, "are n_iterations equal");
	if (new.n_iterations.val == 0)
		return 0;
	for (uint32_t i = 0; i < new.n_iterations.val; i++) {
		assert_iter_equal(i, new, new.iterations[i], expected.iterations[i]);
	}
}
