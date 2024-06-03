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
	return (data.n_chunks.val * 4) + /* light_exposure */
		   (data.n_chunks.val * 4) + /* temp_celsius */
		   (data.n_chunks.val * 4) + /* water_use_ml */
		   (data.n_wavelength.val) + /* light_wavelengt */
		   4 +						 /* pH */
		   4 +						 /* pressure_pa */
		   4 +						 /* soil_density_t_m3 */
		   4 +						 /* nitrogen_g_m3 */
		   4 +						 /* potassium_g_m3 */
		   4 +						 /* phosphorus_g_m3 */
		   4 +						 /* iron_g_m3 */
		   4 +						 /* magnesium_g_m3 */
		   4 +						 /* sulfur_g_m3 */
		   4;						 /* calcium_g_m3 */
}

size_t adf_size(adf_t data)
{
	return 1 +											/* signature */
		   1 +											/* version */
		   4 +											/* n_wavelength */
		   4 +											/* min_w_len_nm */
		   4 +											/* max_w_len_nm */
		   4 +											/* period */
		   4 +											/* n_chunks */
		   4 +											/* n_iterations */
		   (data.n_iterations.val * iter_t_size(data)); /* iterations size */
}

uint8_t *bytes_alloc(adf_t data)
{
	return (uint8_t *)malloc(adf_size(data));
}

adf_bytes marshal(adf_t data)
{
	size_t size_data = adf_size(data);
	size_t byte_c = 0;
	uint8_t *bytes = (uint8_t *)malloc(size_data);
	marshal_number marshal_fn = is_big_endian()
									? &to_big_endian
									: &to_little_endian;
	*(bytes + byte_c) = data.signature;
	byte_c++;
	*(bytes + byte_c) = data.version;
	byte_c++;
	marshal_fn((bytes + byte_c), data.n_wavelength.bytes);
	byte_c += 4;
	marshal_fn((bytes + byte_c), data.min_w_len_nm.bytes);
	byte_c += 4;
	marshal_fn((bytes + byte_c), data.max_w_len_nm.bytes);
	byte_c += 4;
	marshal_fn((bytes + byte_c), data.period.bytes);
	byte_c += 4;
	marshal_fn((bytes + byte_c), data.n_chunks.bytes);
	byte_c += 4;
	marshal_fn((bytes + byte_c), data.n_iterations.bytes);
	byte_c += 4;
	for (uint32_t i = 0, n_iter = data.n_iterations.val; i < n_iter; i++) {
		iter_t current = data.iterations[i];
		if (!current.light_exposure)
			return (adf_bytes){
				.bytes = NULL,
				.code = NOT_OK
			};
		for (uint32_t mask_i = 0; mask_i < data.n_chunks.val; mask_i++, byte_c += 4) {
			marshal_fn((bytes + byte_c), current.light_exposure[mask_i].bytes);
		}
		if (!current.temp_celsius)
			return (adf_bytes){
				.bytes = NULL,
				.code = NOT_OK
			};
		for (uint32_t temp_i = 0; temp_i < data.n_chunks.val; temp_i++, byte_c += 4) {
			marshal_fn((bytes + byte_c), current.temp_celsius[temp_i].bytes);
		}
		if (!current.water_use_ml)
			return (adf_bytes){
				.bytes = NULL,
				.code = NOT_OK
			};
		for (uint32_t w_i = 0; w_i < data.n_chunks.val; w_i++, byte_c += 4) {
			marshal_fn((bytes + byte_c), current.water_use_ml[w_i].bytes);
		}
		if (!current.light_wavelength)
			return (adf_bytes){
				.bytes = NULL,
				.code = NOT_OK
			};
		for (uint32_t wl_i = 0; wl_i < data.n_wavelength.val; wl_i++, byte_c += 1) {
			*(bytes + byte_c) = current.light_wavelength[wl_i];
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
	return (adf_bytes){
		.bytes = bytes,
		.code = OK
	};
}

adf_t *unmarshal(const uint8_t *bytes)
{
	adf_t *adf;
	size_t byte_c = 0;
	unmarshal_number unmarshal_fn = is_big_endian()
										? &to_big_endian
										: &to_little_endian;
	if (!bytes)
		return NULL;
	if (!(adf = malloc(sizeof(adf_t))))
		return NULL;

	adf->signature = *(bytes + byte_c);
	byte_c++;
	adf->version = *(bytes + byte_c);
	byte_c++;
	unmarshal_fn(adf->n_wavelength.bytes, (bytes + byte_c));
	byte_c += 4;
	unmarshal_fn(adf->min_w_len_nm.bytes, (bytes + byte_c));
	byte_c += 4;
	unmarshal_fn(adf->max_w_len_nm.bytes, (bytes + byte_c));
	byte_c += 4;
	unmarshal_fn(adf->period.bytes, (bytes + byte_c));
	byte_c += 4;
	unmarshal_fn(adf->n_chunks.bytes, (bytes + byte_c));
	byte_c += 4;
	unmarshal_fn(adf->n_iterations.bytes, (bytes + byte_c));
	byte_c += 4;
	if (!(adf->iterations = malloc(adf->n_iterations.val * sizeof(iter_t))))
		return NULL;

	for (uint32_t i = 0, n_iter = adf->n_iterations.val; i < n_iter; i++) {
		iter_t current;
		current.light_exposure = malloc(adf->n_chunks.val * sizeof(real_t));
		current.temp_celsius = malloc(adf->n_chunks.val * sizeof(real_t));
		current.water_use_ml = malloc(adf->n_chunks.val * sizeof(real_t));
		current.light_wavelength = malloc(adf->n_wavelength.val * sizeof(uint8_t));

		for (u_int32_t mask_i = 0; mask_i < adf->n_chunks.val; mask_i++, byte_c += 4) {
			unmarshal_fn(current.light_exposure[mask_i].bytes, (bytes + byte_c));
		}
		for (u_int32_t temp_i = 0; temp_i < adf->n_chunks.val; temp_i++, byte_c += 4) {
			unmarshal_fn(current.temp_celsius[temp_i].bytes, (bytes + byte_c));
		}
		for (u_int32_t w_i = 0; w_i < adf->n_chunks.val; w_i++, byte_c += 4) {
			unmarshal_fn(current.water_use_ml[w_i].bytes, (bytes + byte_c));
		}
		for (u_int32_t wl_i = 0; wl_i < adf->n_wavelength.val; wl_i++, byte_c += 1) {
			current.light_wavelength[wl_i] = *(bytes + byte_c);
		}
		byte_c++;
		unmarshal_fn(current.pH.bytes, (bytes + byte_c));
		byte_c += 4;
		unmarshal_fn(current.pressure_pa.bytes, (bytes + byte_c));
		byte_c += 4;
		unmarshal_fn(current.soil_density_t_m3.bytes, (bytes + byte_c));
		byte_c += 4;
		unmarshal_fn(current.nitrogen_g_m3.bytes, (bytes + byte_c));
		byte_c += 4;
		unmarshal_fn(current.potassium_g_m3.bytes, (bytes + byte_c));
		byte_c += 4;
		unmarshal_fn(current.phosphorus_g_m3.bytes, (bytes + byte_c));
		byte_c += 4;
		unmarshal_fn(current.iron_g_m3.bytes, (bytes + byte_c));
		byte_c += 4;
		unmarshal_fn(current.magnesium_g_m3.bytes, (bytes + byte_c));
		byte_c += 4;
		unmarshal_fn(current.sulfur_g_m3.bytes, (bytes + byte_c));
		byte_c += 4;
		unmarshal_fn(current.calcium_g_m3.bytes, (bytes + byte_c));
		byte_c += 4;
		adf->iterations[i] = current;
	}
	return adf;
}
