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

#define FILE_PATH "sample.adf"

void test_marshal_null_bytes(void)
{
	adf_t data;
	uint16_t result = marshal(NULL, &data);
	assert_true(result == ADF_RUNTIME_ERROR,
				"marshal should return error for NULL bytes");
}

void test_marshal_null_data(void)
{
	uint8_t buffer[256];
	uint16_t result = marshal(buffer, NULL);
	assert_true(result == ADF_RUNTIME_ERROR,
				"marshal should return error for NULL data");
}

void marshaled_default_object_equal_to_sample_file(void) 
{
	adf_t adf;
	uint8_t *bytes;
	FILE *sample_file;
	uint8_t *file_bytes;
	uint16_t res;
	long file_len;

	printf(YELLOW WARN_SAMPLE RESET "\n");

	adf = get_default_object();

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

	/* marshalling the object */
	bytes = malloc(size_adf_t(&adf) * sizeof(uint8_t));
	res = marshal(bytes, &adf);
	if (res != ADF_OK) {
		printf("%s", "An error occurred during marshal process\n_chunks");
		exit(0);
	}

	/* compare byte arrays */
	assert_long_equal(file_len, size_adf_t(&adf),
					  "are byte arrays of the same length");
	assert_uint8_arrays_equal(file_bytes, bytes, file_len, 
							  "are byte arrays equal");

	free(file_bytes);
	free(bytes);
	adf_free(&adf);
}

int main(void)
{
	test_marshal_null_bytes();
	test_marshal_null_data();
	marshaled_default_object_equal_to_sample_file();
}
