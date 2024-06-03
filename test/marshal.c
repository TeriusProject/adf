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
#include <stdio.h>

int main()
{
	adf_t format = {
		.signature = __ADF_SIGNATURE__,
		.n_chunks = 10,
		.n_wavelength = 10,
		.n_iterations = 0,
		.iterations = NULL
	};
	adf_bytes res =marshal(format);
	if (res.code != OK) {
		printf("%s", "An error occurred during marshal process\n_chunks");
		return 1;
	}
	uint8_t *bytes = res.bytes;
	printf("%p\n_chunks", bytes);
	printf("Size: %zu bytes\n_chunks", adf_size(format));
	printf("signature: %x\n_chunks", bytes[0]);
	printf("n_chunks: %x\n_chunks", bytes[1]);
	printf("n_wavelength: %d\n_chunks", bytes[2]);
	printf("%p\n_chunks", unmarshal(NULL));
}
