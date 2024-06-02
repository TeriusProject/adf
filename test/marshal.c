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
		.signature = SIGNATURE,
		.n = 10,
		.wavelength_n = 10,
		.n_iterations = 0,
		.iterations = NULL
	};
	uint8_t *bytes = marshal(format);
	printf("%p\n", bytes);
	printf("Size: %zu bytes\n", adf_size(format));
	printf("signature: %x\n", bytes[0]);
	printf("n: %x\n", bytes[1]);
	printf("wavelength_n: %d\n", bytes[2]);
	printf("%p\n", unmarshal(NULL));
}
