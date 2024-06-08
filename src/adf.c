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

#define SHIFT_COUNTER(n) (byte_c += n)

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

static void to_big_endian_4_bytes(uint8_t *dest, const uint8_t *source)
{
	*(dest) = *source;
	*(dest + 1) = *(source + 1);
	*(dest + 2) = *(source + 2);
	*(dest + 3) = *(source + 3);
}

static void to_little_endian_4_bytes(uint8_t *dest, const uint8_t *source)
{
	*(dest) = *(source + 3);
	*(dest + 1) = *(source + 2);
	*(dest + 2) = *(source + 1);
	*(dest + 3) = *source;
}

static void to_big_endian_2_bytes(uint8_t *dest, const uint8_t *source)
{
	*(dest) = *source;
	*(dest + 1) = *(source + 1);
}

static void to_little_endian_2_bytes(uint8_t *dest, const uint8_t *source)
{
	*(dest) = *(source + 1);
	*(dest + 1) = *source;
}

size_t size_iter_t(adf_t data)
{
	return (data.n_chunks.val * 4) +	 /* light_exposure */
		   (data.n_chunks.val * 4) +	 /* temp_celsius */
		   (data.n_chunks.val * 4) +	 /* water_use_ml */
		   (data.n_wavelength.val * 4) + /* light_wavelengt */
		   4 +							 /* pH */
		   4 +							 /* pressure_pa */
		   4 +							 /* soil_density_t_m3 */
		   4 +							 /* nitrogen_g_m3 */
		   4 +							 /* potassium_g_m3 */
		   4 +							 /* phosphorus_g_m3 */
		   4 +							 /* iron_g_m3 */
		   4 +							 /* magnesium_g_m3 */
		   4 +							 /* sulfur_g_m3 */
		   4;							 /* calcium_g_m3 */
}

size_t size_adf_t(adf_t data)
{
	return 4 +										/* signature */
		   1 +										/* version */
		   4 +										/* n_wavelength */
		   4 +										/* min_w_len_nm */
		   4 +										/* max_w_len_nm */
		   4 +										/* period_sec */
		   4 +										/* n_chunks */
		   4 +										/* n_series */
		   (data.n_series.val * size_iter_t(data)); /* iterations size */
}

uint8_t *bytes_alloc(adf_t data)
{
	return (uint8_t *)malloc(size_adf_t(data));
}

static marshal_number cpy_4_bytes_fn = is_big_endian()
									  ? &to_big_endian_4_bytes
									  : &to_little_endian_4_bytes;
static marshal_number cpy_2_bytes_fn = is_big_endian()
									  ? &to_big_endian_2_bytes
									  : &to_little_endian_2_bytes;

long marshal(uint8_t *bytes, adf_t data)
{
	size_t byte_c = 0;

	if (!bytes)
		return NOT_OK;
	cpy_4_bytes_fn((bytes + byte_c), data.signature.bytes);
	SHIFT_COUNTER(4);
	*(bytes + byte_c) = data.version;
	SHIFT_COUNTER(1);
	*(bytes + byte_c) = data.farming_tecnique;
	SHIFT_COUNTER(1);
	cpy_4_bytes_fn((bytes + byte_c), data.n_wavelength.bytes);
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn((bytes + byte_c), data.min_w_len_nm.bytes);
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn((bytes + byte_c), data.max_w_len_nm.bytes);
	SHIFT_COUNTER();
	cpy_4_bytes_fn((bytes + byte_c), data.n_chunks.bytes);
	SHIFT_COUNTER(4);
	cpy_2_bytes_fn((bytes + byte_c), data.crc.bytes);
	SHIFT_COUNTER(2);
	cpy_4_bytes_fn((bytes + byte_c), data.metadata.n_series.bytes);
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn((bytes + byte_c), data.metadata.period_sec.bytes);
	SHIFT_COUNTER(4);
	cpy_2_bytes_fn((bytes + byte_c), data.metadata.n_additives.bytes);
	SHIFT_COUNTER(2);
	for (uint16_t i = 0, l = data.metadata.n_additives.val; i < l; i++, byte_c+=4) {
		cpy_4_bytes_fn((bytes + byte_c), data.metadata.additive_codes[i].bytes);
	}

	for (uint32_t i = 0, n_iter = data.metadata.n_series.val; i < n_iter; i++) {
		series_t current = data.series[i];
		if (!current.light_exposure)
			return NOT_OK;
		for (uint32_t mask_i = 0; mask_i < data.n_chunks.val; mask_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c), current.light_exposure[mask_i].bytes);
		}
		if (!current.temp_celsius)
			return NOT_OK;
		for (uint32_t temp_i = 0; temp_i < data.n_chunks.val; temp_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c), current.temp_celsius[temp_i].bytes);
		}
		if (!current.water_use_ml)
			return NOT_OK;
		for (uint32_t w_i = 0; w_i < data.n_chunks.val; w_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c), current.water_use_ml[w_i].bytes);
		}
		*(bytes + byte_c) = current.pH;
		SHIFT_COUNTER(1);
		cpy_4_bytes_fn((bytes + byte_c), current.p_bar.bytes);
		SHIFT_COUNTER(4);
		cpy_4_bytes_fn((bytes + byte_c), current.soil_density.bytes);
		SHIFT_COUNTER(4);
		cpy_4_bytes_fn((bytes + byte_c), current.n_soil_adds.bytes);
		SHIFT_COUNTER(2);
		cpy_4_bytes_fn((bytes + byte_c), current.n_atm_adds.bytes);
		SHIFT_COUNTER(2);
		for (uint16_t j = 0, l = current.n_soil_adds.val; j < l; j++, byte_c += 6) {
			cpy_2_bytes_fn((bytes + byte_c), current.soil_additives[j].code.bytes);
			cpy_4_bytes_fn((bytes + byte_c), current.soil_additives[j].concentration.bytes);
		}
		for (uint16_t j = 0, l = current.n_atm_adds.val; j < l; j++, byte_c += 6) {
			cpy_2_bytes_fn((bytes + byte_c), current.atm_additives[j].code.bytes);
			cpy_4_bytes_fn((bytes + byte_c), current.atm_additives[j].concentration.bytes);
		}
		cpy_4_bytes_fn((bytes + byte_c), current.repeated.bytes);
	}
	return OK;
}

long unmarshal(adf_t *adf, const uint8_t *bytes)
{
	size_t byte_c = 0;

	if (!bytes || !adf)
		return NOT_OK;

	cpy_4_bytes_fn(adf->signature.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	adf->version = *(bytes + byte_c);
	SHIFT_COUNTER(1);
	adf->farming_tecnique = *(bytes + byte_c);
	SHIFT_COUNTER(1);
	cpy_4_bytes_fn(adf->n_wavelength.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn(adf->min_w_len_nm.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn(adf->max_w_len_nm.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn(adf->n_chunks.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_2_bytes_fn(adf->crc.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);
	cpy_4_bytes_fn(adf->metadata.n_series.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn(adf->metadata.period_sec.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_2_bytes_fn(adf->metadata.n_additives.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);
	for (uint16_t i = 0, l = adf->metadata.n_additives.val; i< l; i++, byte_c += 4) {
		cpy_2_bytes_fn(adf->metadata.additive_codes[i].bytes, (bytes + byte_c));
	}

	if (!(adf->series = malloc(adf->metadata.n_series.val * sizeof(series_t))))
		return NOT_OK;

	for (uint32_t i = 0, n_iter = adf->n_series.val; i < n_iter; i++) {
		series_t current;
		current.light_exposure = malloc(adf->n_chunks.val * sizeof(real_t));
		current.temp_celsius = malloc(adf->n_chunks.val * sizeof(real_t));
		current.water_use_ml = malloc(adf->n_chunks.val * sizeof(real_t));

		for (u_int32_t mask_i = 0; mask_i < adf->n_chunks.val; mask_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.light_exposure[mask_i].bytes, (bytes + byte_c));
		}
		for (u_int32_t temp_i = 0; temp_i < adf->n_chunks.val; temp_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.temp_celsius[temp_i].bytes, (bytes + byte_c));
		}
		for (u_int32_t w_i = 0; w_i < adf->n_chunks.val; w_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.water_use_ml[w_i].bytes, (bytes + byte_c));
		}
		current.pH = *(bytes + byte_c);
		SHIFT_COUNTER(1);
		cpy_4_bytes_fn(current.p_bar.bytes, (bytes + byte_c));
		SHIFT_COUNTER(4);
		cpy_4_bytes_fn(current.soil_density.bytes, (bytes + byte_c));
		SHIFT_COUNTER(4);
		cpy_2_bytes_fn(current.n_soil_adds.bytes, (bytes + byte_c));
		SHIFT_COUNTER(2);
		cpy_2_bytes_fn(current.n_atm_adds.bytes, (bytes + byte_c));
		SHIFT_COUNTER(2);
		for (uint16_t j = 0, l = current.n_soil_adds.val; j < l; j++, byte_c += 6) {
			cpy_2_bytes_fn(current.soil_additives[j].code.bytes,(bytes + byte_c));
			cpy_4_bytes_fn(current.soil_additives[j].concentration.bytes, (bytes + byte_c));
		}
		for (uint16_t j = 0, l = current.n_atm_adds.val; j < l; j++, byte_c += 6) {
			cpy_2_bytes_fn(current.atm_additives[j].code.bytes,(bytes + byte_c));
			cpy_4_bytes_fn(current.atm_additives[j].concentration.bytes, (bytes + byte_c));
		}
		adf->iterations[i] = current;
	}
	return OK;
}

long add_series(adf_t * adf, const series_t * series)
{
}