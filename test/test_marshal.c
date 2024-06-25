/* test_marshal.c
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
#include <adf.h>
#include <stdio.h>

#define FILE_PATH "sample.adf"

int main(void)
{
	adf_t obj = get_default_object();
	uint8_t *bytes = malloc(size_adf_t(obj) * sizeof(uint8_t));
	FILE *sample_file;
	uint8_t *file_bytes;
	uint16_t res;
	long file_len;

	/* reading the file with expected bytes */
	sample_file = fopen(FILE_PATH, "rb");
	if (sample_file == NULL) {
		printf("The file `%s` is not opened.", FILE_PATH);
		exit(0);
	}
	fseek(sample_file, 0, SEEK_END);
	file_len = ftell(sample_file);
	rewind(sample_file);
	file_bytes = (uint8_t *)malloc(file_len * sizeof(uint8_t));
	fread(file_bytes, file_len, 1, sample_file);
	fclose(sample_file);
	free(file_bytes);

	/* marshalling the object */
	bytes = malloc(size_adf_t(obj) * sizeof(uint8_t));
	res = marshal(bytes, obj);
	if (res != ADF_OK) {
		printf("%s", "An error occurred during marshal process\n_chunks");
		return 1;
	}

	/* compare byte arrays */
	assert_long_equal(
		file_len, size_adf_t(obj), "are byte arrays of the same length"
	);
	assert_uint8_arrays_equal(
		file_bytes, bytes, file_len, "are byte arrays equal"
	);
}
