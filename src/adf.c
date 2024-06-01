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

#include <stdlib.h>
#include "adf.h"

static _Bool is_big_endian()
{
	union {
		uint32_t val;
		uint8_t bytes[4];
	} endianess = {0x01000000};

	return endianess.bytes[0];
}

typedef void (*marshal_real)(real_t, uint8_t *);
typedef real_t (*unmarshal_real)(uint8_t *);

static void marshal_real_big_endian(real_t n, uint8_t *bytes)
{
	*(bytes) = n.bytes[0];
	*(bytes + 1) = n.bytes[1];
	*(bytes + 2) = n.bytes[2];
	*(bytes + 3) = n.bytes[3];
}

static void marshal_real_little_endian(real_t n, uint8_t *bytes)
{
	*(bytes) = n.bytes[3];
	*(bytes + 1) = n.bytes[2];
	*(bytes + 2) = n.bytes[1];
	*(bytes + 3) = n.bytes[0];
}

static size_t iter_t_size(adf_t data)
{
	return 41 + (data.wavelength_n * 4) + (data.n * 8) + 24;
}

size_t adf_size(adf_t data)
{
	return 4 + (data.n_iterations * iter_t_size(data));
}

uint8_t *marshal(adf_t data)
{
	uint8_t *bytes = (uint8_t *)malloc(adf_size(data));
	marshal_real marshal_fn = is_big_endian()
								  ? &marshal_real_big_endian
								  : &marshal_real_little_endian;
	*(bytes) = data.signature;
	*(bytes + 1) = data.n;
	*(bytes + 2) = data.wavelength_n;
	*(bytes + 3) = data.n_iterations;
	return NULL;
}

adf_t *unmarshal(const uint8_t *bytes)
{
	(void)bytes;
	return NULL;
}
