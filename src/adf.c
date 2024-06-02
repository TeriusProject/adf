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

#include "adf.h"
#include <stdlib.h>

static _Bool is_big_endian()
{
	union {
		uint32_t val;
		uint8_t bytes[4];
	} endianess = {0x01000000};

	return endianess.bytes[0];
}

typedef void (*marshal_number)(uint8_t *, const uint8_t *);
typedef void (*unmarshal_number)(uint8_t *, const uint8_t *);

static void to_big_endian(uint8_t *dest, const uint8_t *source)
{
	*(dest) = *source;
	*(dest + 1) = *(source + 1);
	*(dest + 2) = *(source + 2);
	*(dest + 3) = *(source + 3);
}

static void to_little_endian(uint8_t *dest, const uint8_t *source)
{
	*(dest) = *(source + 3);
	*(dest + 1) = *(source + 2);
	*(dest + 2) = *(source + 1);
	*(dest + 3) = *source;
}

static size_t iter_t_size(adf_t data)
{
	return 41 + (data.wavelength_n * 4) + (data.n * 8) + 24;
}

size_t adf_size(adf_t data)
{
	return 3 + /*  */
		   4 +
		   (data.n_iterations.val * iter_t_size(data)); /* iterations size */
}

uint8_t *marshal(adf_t data)
{
	size_t size_data = adf_size(data);
	size_t byte_c;
	uint8_t *bytes = (uint8_t *)malloc(size_data);
	marshal_number marshal_fn = is_big_endian()
									? &to_big_endian
									: &to_little_endian;
	*(bytes) = data.signature;
	*(bytes + 1) = data.n;
	*(bytes + 2) = data.wavelength_n;
	marshal_fn((bytes + 3), data.n_iterations.bytes);
	byte_c = 7;
	for (uint32_t i = 0, n_iter = data.n_iterations.val; i < n_iter; i++) {
		iter_t current = data.iterations[i];
		for (uint8_t mask_i = 0; mask_i < data.n; mask_i++, byte_c += 4) {
			marshal_fn((bytes + byte_c), current.light_mask[mask_i].bytes);
		}
		for (uint8_t temp_i = 0; temp_i < data.n; temp_i++, byte_c += 4) {
			marshal_fn((bytes + byte_c), current.temp_celsius[temp_i].bytes);
		}
		for (uint8_t wl_i = 0; wl_i < data.wavelength_n; wl_i++, byte_c += 1) {
			*(bytes + byte_c) = current.light_wavelength[wl_i];
		}
		for (uint8_t w_i = 0; w_i < data.wavelength_n; w_i++, byte_c += 1) {
			*(bytes + byte_c) = current.water_use_ml[w_i];
		}
		byte_c++;
		marshal_fn((bytes + byte_c), current.pH.bytes);
		byte_c += 4;
		marshal_fn((bytes + byte_c), current.pressure_pa.bytes);
		byte_c += 4;
		marshal_fn((bytes + byte_c), current.soil_density_t_m3.bytes);
		byte_c += 4;
		marshal_fn((bytes + byte_c), current.nitrogen_g_m3.bytes);
		byte_c += 4;
		marshal_fn((bytes + byte_c), current.potassium_g_m3.bytes);
		byte_c += 4;
		marshal_fn((bytes + byte_c), current.phosphorus_g_m3.bytes);
		byte_c += 4;
		marshal_fn((bytes + byte_c), current.iron_g_m3.bytes);
		byte_c += 4;
		marshal_fn((bytes + byte_c), current.magnesium_g_m3.bytes);
		byte_c += 4;
		marshal_fn((bytes + byte_c), current.sulfur_g_m3.bytes);
		byte_c += 4;
		marshal_fn((bytes + byte_c), current.calcium_g_m3.bytes);
		byte_c += 4;
	}
	return bytes;
}

adf_t *unmarshal(const uint8_t *bytes)
{
	adf_t *adf;
	iter_t *iterations;
	size_t byte_c;
	unmarshal_number unmarshal_fn = is_big_endian()
										? &to_big_endian
										: &to_little_endian;
	if (!bytes)
		return NULL;
	if (!(adf = malloc(sizeof(adf_t))))
		return NULL;

	adf->signature = *bytes;
	adf->n = *(bytes + 1);
	adf->wavelength_n = *(bytes + 2);
	unmarshal_fn(adf->n_iterations.bytes, (bytes + 3));
	byte_c = 7;
	if (!(iterations = malloc(adf->n_iterations.val * sizeof(iter_t))))
		return NULL;

	for (uint32_t i = 0, n_iter = adf->n_iterations.val; i < n_iter; i++) {
		//TODO: deserialize iterations
		iter_t current;
		for (uint8_t mask_i = 0; mask_i < adf->n; mask_i++, byte_c += 4) {
			marshal_fn(current.light_mask[mask_i].bytes, (bytes + byte_c));
		}
	}

	return adf;
}
