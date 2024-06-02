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
	adf_t *new = unmarshal(bytes);
	printf("%p\n", new);
	printf("Size: %zu bytes\n", adf_size(*new));
	printf("signature: %x\n", new->signature);
	printf("n: %d\n", new->n);
	printf("wavelength_n: %d\n", new->wavelength_n);
	printf("n_iterations: %d\n", new->n_iterations.val);
}
