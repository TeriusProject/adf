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
	union
	{
		uint32_t val;
		uint8_t bytes[4];
	} endianess = {0x01000000};

	return endianess.bytes[0];
}

typedef void (*marshal_number)(uint8_t *, uint8_t *);
typedef real_t (*unmarshal_real)(uint8_t *);

static void marshal_number_big_endian(uint8_t *source, uint8_t *dest)
{
	*(dest) = source[0];
	*(dest + 1) = source[1];
	*(dest + 2) = source[2];
	*(dest + 3) = source[3];
}

static void marshal_number_little_endian(real_t n, uint8_t *bytes)
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
	return 3 +  /*  */
		   4 +
		   (data.n_iterations.val * iter_t_size(data));  /* iterations size */
}

uint8_t *marshal(adf_t data)
{
	size_t size_data = adf_size(data);
	uint8_t *bytes = (uint8_t *)malloc(size_data);
	marshal_number marshal_fn = is_big_endian()
									? &marshal_number_big_endian
									: &marshal_number_little_endian;
	*(bytes) = data.signature;
	*(bytes + 1) = data.n;
	*(bytes + 2) = data.wavelength_n;
	marshal_fn(*(bytes + 3), data.n_iterations.bytes);
	size_t byte_counter = 7;
	for (uint32_t i = 0, n_iter = data.n_iterations.val; i < n_iter; i++) {
		iter_t current = data.iterations[i];
		for (uint8_t mask_i = 0; mask_i < data.n; mask_i++, byte_counter += 4) {
			marshal_fn(*(bytes+byte_counter),current.light_mask[mask_i].bytes);
		}
		for (uint8_t temp_i = 0; temp_i < data.n; temp_i++, byte_counter += 4) {
			marshal_fn(*(bytes+byte_counter),current.temperatures_cent[temp_i].bytes);
		}
		for (uint8_t wavelength_i = 0; wavelength_i < data.wavelength_n; wavelength_i++, byte_counter += 1) {
			*(bytes+byte_counter) = current.light_wavelength[wavelength_i];
		}
		for (uint8_t water_i = 0; water_i < data.wavelength_n; water_i++, byte_counter += 1) {
			*(bytes+byte_counter) = current.water_use_ml[water_i];
		}
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		marshal_fn(*(bytes + 3), data.n_iterations.bytes);
		byte_counter += 40;
	}
		return bytes;
	}

	adf_t *unmarshal(const uint8_t *bytes)
	{
		(void)bytes;
		return NULL;
	}
