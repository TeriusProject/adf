/* 
 * generate_sample.c - Generate the file `sample.adf`, required by some 
 * test suites
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
#include <stdio.h>
#include <stdlib.h>

#define FILE_PATH "sample.adf"

int main(void)
{
	uint16_t res;
	adf_t expected = get_default_object();
	uint8_t *bytes = malloc(size_adf_t(&expected) * sizeof(uint8_t));
	if((res = marshal(bytes, &expected)) != ADF_OK) {
		printf("[%x] %s", res, "An error occurred during marshal process\n");
		exit(1);
	}
	FILE *sample_file = fopen(FILE_PATH, "wb");
	fwrite(bytes, 1, size_adf_t(&expected), sample_file);
	fclose(sample_file);
}
