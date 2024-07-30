/*
 * adf.c
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

#include "adf.h"
#include "crc.h"
#include "lookup_table.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SHIFT1(byte_counter) (byte_counter++)
#define SHIFT2(byte_counter) (byte_counter += 2)
#define SHIFT4(byte_counter) (byte_counter += 4)
#define DEBUG_STR "*** DEBUG *** "

typedef void (*number_bytes_copy)(uint8_t *, const uint8_t *);

static number_bytes_copy cpy_4_bytes_fn;
static number_bytes_copy cpy_2_bytes_fn;

static bool is_big_endian(void)
{
	union {
		uint16_t val;
		uint8_t bytes[2];
	} endianess = { 0x0100 };

	return endianess.bytes[0];
}

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

uint8_t get_version(void) { return __ADF_VERSION__; }

size_t size_series_t(adf_t *adf, series_t *series)
{
	uint32_t n_chunks = adf->header.n_chunks.val;
	uint32_t n_wave = adf->header.n_wavelength.val;
	uint32_t n_depth = adf->header.n_depth.val;
	uint16_t n_soil_add = series->n_soil_adds.val;
	uint16_t n_atm_add = series->n_atm_adds.val;
	return (n_wave * n_chunks * REAL_T_SIZE)	/* light_exposure */
		   + (n_depth * n_chunks * REAL_T_SIZE)	/* soil_temp_c */
		   + (n_chunks * REAL_T_SIZE)			/* env_temp_c */
		   + (n_chunks * REAL_T_SIZE)			/* water_use_ml */
		   + UINT_TINY_T_SIZE					/* pH */
		   + REAL_T_SIZE						/* p_bar */
		   + REAL_T_SIZE						/* soil_density_t_m3 */
		   + UINT_SMALL_T_SIZE					/* n_soil_adds */
		   + UINT_SMALL_T_SIZE					/* n_atm_adds */
		   + (ADD_T_SIZE * n_soil_add)			/* soil_additives */
		   + (ADD_T_SIZE * n_atm_add)			/* atm_additives */
		   + UINT_T_SIZE						/* repeated */
		   + UINT_SMALL_T_SIZE;					/* crc */
}

size_t size_medatata_t(adf_meta_t *metadata)
{
	uint16_t add_codes_size = metadata->n_additives.val;
	return UINT_T_SIZE							/* size_series */
		   + UINT_T_SIZE						/* period_sec */
		   + UINT_T_SIZE						/* seeded */
		   + UINT_T_SIZE						/* harvested */
		   + UINT_SMALL_T_SIZE					/* n_additives */
		   + (add_codes_size * UINT_T_SIZE)		/* additive_codes */
		   + UINT_TINY_T_SIZE					/* soil_density_red_mode */
		   + UINT_TINY_T_SIZE					/* pressure_red_mode */
		   + UINT_TINY_T_SIZE					/* light_exposure_red_mode */
		   + UINT_TINY_T_SIZE					/* water_use_red_mode */
		   + UINT_TINY_T_SIZE					/* soil_temp_red_mode */
		   + UINT_TINY_T_SIZE					/* env_temp_red_mode */
		   + UINT_SMALL_T_SIZE;					/* crc */
}

size_t size_header(void)
{
	return UINT_T_SIZE			/* signature */
		   + UINT_SMALL_T_SIZE	/* version */
		   + UINT_TINY_T_SIZE	/* farming_tec */
		   + UINT_SMALL_T_SIZE	/* n_wavelength */
		   + UINT_SMALL_T_SIZE	/* min_w_len_nm */
		   + UINT_SMALL_T_SIZE	/* max_w_len_nm */
		   + UINT_SMALL_T_SIZE	/* n_depth */
		   + UINT_SMALL_T_SIZE	/* min_soil_depth_mm */
		   + UINT_SMALL_T_SIZE	/* max_soil_depth_mm */
		   + UINT_T_SIZE		/* n_chunks */
		   + UINT_SMALL_T_SIZE;	/* crc */
}

size_t size_adf_t(adf_t data)
{
	const size_t head_metadata_size
		= size_header() + size_medatata_t(&data.metadata);
	size_t series_size = 0;
	for (uint32_t i = 0, l = data.metadata.size_series.val; i < l; i++) {
		series_size += size_series_t(&data, data.series + i);
	}
	return head_metadata_size + series_size;
}

uint8_t *adf_bytes_alloc(adf_t data) {
	return (uint8_t *)malloc(size_adf_t(data));
}

void adf_bytes_free(uint8_t *bytes) {
	free(bytes);
	bytes = NULL;
}

uint16_t marshal(uint8_t *bytes, adf_t data)
{
	size_t byte_c = 0;
	uint_small_t crc_16bits;
	cpy_4_bytes_fn = is_big_endian()
					 ? &to_big_endian_4_bytes 
					 : &to_little_endian_4_bytes;
	cpy_2_bytes_fn = is_big_endian()
					 ? &to_big_endian_2_bytes 
					 : &to_little_endian_2_bytes;
	if (!bytes) { return ADF_RUNTIME_ERROR; }
	cpy_4_bytes_fn((bytes + byte_c), data.header.signature.bytes);
	SHIFT4(byte_c);
	cpy_2_bytes_fn(bytes + byte_c, data.header.version.bytes);
	SHIFT2(byte_c);
	*(bytes + byte_c) = data.header.farming_tec;
	SHIFT1(byte_c);
	cpy_2_bytes_fn((bytes + byte_c), data.header.n_wavelength.bytes);
	SHIFT2(byte_c);
	cpy_2_bytes_fn((bytes + byte_c), data.header.min_w_len_nm.bytes);
	SHIFT2(byte_c);
	cpy_2_bytes_fn((bytes + byte_c), data.header.max_w_len_nm.bytes);
	SHIFT2(byte_c);
	cpy_2_bytes_fn((bytes + byte_c), data.header.n_depth.bytes);
	SHIFT2(byte_c);
	cpy_2_bytes_fn((bytes + byte_c), data.header.min_soil_depth_mm.bytes);
	SHIFT2(byte_c);
	cpy_2_bytes_fn((bytes + byte_c), data.header.max_soil_depth_mm.bytes);
	SHIFT2(byte_c);
	cpy_4_bytes_fn((bytes + byte_c), data.header.n_chunks.bytes);
	SHIFT4(byte_c);
	crc_16bits.val = crc16(bytes, byte_c);
	cpy_2_bytes_fn((bytes + byte_c), crc_16bits.bytes);
	SHIFT2(byte_c);

	#ifdef __ADF_DEBUG__
	printf(DEBUG_STR "Marshal header done\n");
	#endif /* __ADF_DEBUG__ */

	cpy_4_bytes_fn((bytes + byte_c), data.metadata.size_series.bytes);
	SHIFT4(byte_c);
	cpy_4_bytes_fn((bytes + byte_c), data.metadata.period_sec.bytes);
	SHIFT4(byte_c);
	cpy_4_bytes_fn((bytes + byte_c), data.metadata.seeded.bytes);
	SHIFT4(byte_c);
	cpy_4_bytes_fn((bytes + byte_c), data.metadata.harvested.bytes);
	SHIFT4(byte_c);
	cpy_2_bytes_fn((bytes + byte_c), data.metadata.n_additives.bytes);
	SHIFT2(byte_c);
	*(bytes + byte_c) = data.metadata.soil_density_red_mode;
	SHIFT1(byte_c);
	*(bytes + byte_c) = data.metadata.pressure_red_mode;
	SHIFT1(byte_c);
	*(bytes + byte_c) = data.metadata.light_exposure_red_mode;
	SHIFT1(byte_c);
	*(bytes + byte_c) = data.metadata.water_use_red_mode;
	SHIFT1(byte_c);
	*(bytes + byte_c) = data.metadata.soil_temp_red_mode;
	SHIFT1(byte_c);
	*(bytes + byte_c) = data.metadata.env_temp_red_mode;
	SHIFT1(byte_c);

	for (uint16_t i = 0, l = data.metadata.n_additives.val; i < l;
		 i++, byte_c += 4) {
		cpy_4_bytes_fn((bytes + byte_c), data.metadata.additive_codes[i].bytes);
	}

	crc_16bits.val = crc16((bytes + size_header()), byte_c - size_header());
	cpy_2_bytes_fn((bytes + byte_c), crc_16bits.bytes);
	SHIFT2(byte_c);

	#ifdef __ADF_DEBUG__
	printf(DEBUG_STR "Marshal metadata done\n");
	#endif /* __ADF_DEBUG__ */

	uint32_t n_iter = data.metadata.size_series.val;
	uint32_t n_wave = data.header.n_wavelength.val;
	uint32_t n_depth = data.header.n_depth.val;
	for (uint32_t i = 0; i < n_iter; i++) {
		series_t current = data.series[i];
		size_t starting_byte = byte_c;
		if (!current.light_exposure) { return ADF_RUNTIME_ERROR; }
		for (uint32_t l_row = 0; l_row < data.header.n_chunks.val; l_row++) {
			real_t *current_row = *(current.light_exposure + l_row);
			for (uint16_t mask_i = 0; mask_i < n_wave; mask_i++, byte_c += 4) {
				cpy_4_bytes_fn((bytes + byte_c), current_row[mask_i].bytes);
			}
		}
		if (!current.soil_temp_c) { return ADF_RUNTIME_ERROR; }
		for (uint32_t t_row = 0; t_row < data.header.n_chunks.val; t_row++) {
			real_t *current_row = *(current.soil_temp_c + t_row);
			for (uint16_t mask_i = 0; mask_i < n_depth; mask_i++, byte_c += 4) {
				cpy_4_bytes_fn((bytes + byte_c), current_row[mask_i].bytes);
			}
		}
		if (!current.env_temp_c) { return ADF_RUNTIME_ERROR; }
		for (uint32_t temp_i = 0; temp_i < data.header.n_chunks.val;
			 temp_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c),
						   current.env_temp_c[temp_i].bytes);
		}
		if (!current.water_use_ml) { return ADF_RUNTIME_ERROR; }
		for (uint32_t w_i = 0; w_i < data.header.n_chunks.val;
			 w_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c), current.water_use_ml[w_i].bytes);
		}
		*(bytes + byte_c) = current.pH;
		SHIFT1(byte_c);
		cpy_4_bytes_fn((bytes + byte_c), current.p_bar.bytes);
		SHIFT4(byte_c);
		cpy_4_bytes_fn((bytes + byte_c), current.soil_density_kg_m3.bytes);
		SHIFT4(byte_c);
		cpy_2_bytes_fn((bytes + byte_c), current.n_soil_adds.bytes);
		SHIFT2(byte_c);
		cpy_2_bytes_fn((bytes + byte_c), current.n_atm_adds.bytes);
		SHIFT2(byte_c);
		for (uint16_t j = 0, l = current.n_soil_adds.val; j < l; j++) {
			cpy_2_bytes_fn((bytes + byte_c),
						   current.soil_additives[j].code_idx.bytes);
			SHIFT2(byte_c);
			cpy_4_bytes_fn((bytes + byte_c),
						   current.soil_additives[j].concentration.bytes);
			SHIFT4(byte_c);
		}
		for (uint16_t j = 0, l = current.n_atm_adds.val; j < l; j++) {
			cpy_2_bytes_fn((bytes + byte_c),
						   current.atm_additives[j].code_idx.bytes);
			SHIFT2(byte_c);
			cpy_4_bytes_fn((bytes + byte_c),
						   current.atm_additives[j].concentration.bytes);
			SHIFT4(byte_c);
		}
		cpy_4_bytes_fn((bytes + byte_c), current.repeated.bytes);
		SHIFT4(byte_c);

		crc_16bits.val = crc16((bytes + starting_byte), byte_c - starting_byte);
		cpy_2_bytes_fn((bytes + byte_c), crc_16bits.bytes);
		SHIFT2(byte_c);

		#ifdef __ADF_DEBUG__
		printf(DEBUG_STR "Marshal series #%u done\n", i);
		#endif /* __ADF_DEBUG__ */
	}
	return ADF_OK;
}

uint16_t unmarshal(adf_t *adf, const uint8_t *bytes)
{
	size_t byte_c = 0;
	uint_small_t expected_crc;
	uint16_t header_crc, meta_crc, series_crc;
	uint32_t n_series = 0, n_iter, n_chunks, n_waves, n_depth;
	cpy_4_bytes_fn = is_big_endian()
					 ? &to_big_endian_4_bytes 
					 : &to_little_endian_4_bytes;
	cpy_2_bytes_fn = is_big_endian()
					 ? &to_big_endian_2_bytes 
					 : &to_little_endian_2_bytes;

	if (!bytes || !adf) { return ADF_RUNTIME_ERROR; }

	cpy_4_bytes_fn(adf->header.signature.bytes, (bytes + byte_c));
	SHIFT4(byte_c);
	cpy_2_bytes_fn(adf->header.version.bytes, bytes + byte_c);
	SHIFT2(byte_c);
	adf->header.farming_tec = *(bytes + byte_c);
	SHIFT1(byte_c);
	cpy_2_bytes_fn(adf->header.n_wavelength.bytes, (bytes + byte_c));
	SHIFT2(byte_c);
	cpy_2_bytes_fn(adf->header.min_w_len_nm.bytes, (bytes + byte_c));
	SHIFT2(byte_c);
	cpy_2_bytes_fn(adf->header.max_w_len_nm.bytes, (bytes + byte_c));
	SHIFT2(byte_c);
	cpy_2_bytes_fn(adf->header.n_depth.bytes, (bytes + byte_c));
	SHIFT2(byte_c);
	cpy_2_bytes_fn(adf->header.min_soil_depth_mm.bytes, (bytes + byte_c));
	SHIFT2(byte_c);
	cpy_2_bytes_fn(adf->header.max_soil_depth_mm.bytes, (bytes + byte_c));
	SHIFT2(byte_c);
	cpy_4_bytes_fn(adf->header.n_chunks.bytes, (bytes + byte_c));
	SHIFT4(byte_c);
	header_crc = crc16(bytes, byte_c);
	cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
	SHIFT2(byte_c);

	if (header_crc != expected_crc.val) { return ADF_HEADER_CORRUPTED; }

	#ifdef __ADF_DEBUG__
	printf(DEBUG_STR "Unmarshal header done\n");
	#endif /* __ADF_DEBUG__ */

	cpy_4_bytes_fn(adf->metadata.size_series.bytes, (bytes + byte_c));
	SHIFT4(byte_c);
	n_series = adf->metadata.size_series.val;
	cpy_4_bytes_fn(adf->metadata.period_sec.bytes, (bytes + byte_c));
	SHIFT4(byte_c);
	cpy_4_bytes_fn(adf->metadata.seeded.bytes, (bytes + byte_c));
	SHIFT4(byte_c);
	cpy_4_bytes_fn(adf->metadata.harvested.bytes, (bytes + byte_c));
	SHIFT4(byte_c);
	cpy_2_bytes_fn(adf->metadata.n_additives.bytes, (bytes + byte_c));
	SHIFT2(byte_c);
	adf->metadata.soil_density_red_mode = *(bytes + byte_c);
	SHIFT1(byte_c);
	adf->metadata.pressure_red_mode = *(bytes + byte_c);
	SHIFT1(byte_c);
	adf->metadata.light_exposure_red_mode = *(bytes + byte_c);
	SHIFT1(byte_c);
	adf->metadata.water_use_red_mode = *(bytes + byte_c);
	SHIFT1(byte_c);
	adf->metadata.soil_temp_red_mode = *(bytes + byte_c);
	SHIFT1(byte_c);
	adf->metadata.env_temp_red_mode = *(bytes + byte_c);
	SHIFT1(byte_c);

	adf->metadata.additive_codes = malloc(adf->metadata.n_additives.val 
										  * sizeof(uint_t));
	if (!adf->metadata.additive_codes) { return ADF_RUNTIME_ERROR; }

	for (uint16_t i = 0, l = adf->metadata.n_additives.val; i < l;
		 i++, byte_c += 4) {
		cpy_4_bytes_fn(adf->metadata.additive_codes[i].bytes, (bytes + byte_c));
	}

	meta_crc = crc16((bytes + size_header()), byte_c - size_header());
	cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
	SHIFT2(byte_c);
	if (meta_crc != expected_crc.val) { return ADF_METADATA_CORRUPTED; }

	#ifdef __ADF_DEBUG__
	printf(DEBUG_STR "Unmarshal metadata done\n");
	#endif /* __ADF_DEBUG__ */

	adf->series = malloc(adf->metadata.size_series.val * sizeof(series_t));
	if (!adf->series) { return ADF_RUNTIME_ERROR; }

	n_iter = adf->metadata.size_series.val;
	n_chunks = adf->header.n_chunks.val;
	n_waves = adf->header.n_wavelength.val;
	n_depth = adf->header.n_depth.val;

	for (uint32_t i = 0; i < n_iter; i++) {
		series_t current;
		size_t starting_byte = byte_c;
		current.light_exposure = malloc(n_chunks * sizeof(real_t *));
		if (!current.light_exposure) { return ADF_RUNTIME_ERROR; }

		current.soil_temp_c = malloc(n_chunks * sizeof(real_t *));
		if (!current.soil_temp_c) { return ADF_RUNTIME_ERROR; }

		current.env_temp_c = malloc(n_chunks * sizeof(real_t));
		if (!current.env_temp_c) { return ADF_RUNTIME_ERROR; }

		current.water_use_ml = malloc(n_chunks * sizeof(real_t));
		if (!current.water_use_ml) { return ADF_RUNTIME_ERROR; }

		for (uint32_t l_row = 0; l_row < n_chunks; l_row++) {
			current.light_exposure[l_row] = malloc(n_waves * sizeof(real_t));
			for (uint32_t mask_i = 0; mask_i < n_waves; mask_i++, byte_c += 4) {
				cpy_4_bytes_fn(current.light_exposure[l_row][mask_i].bytes,
							   bytes + byte_c);
			}
		}
		for (uint32_t t_row = 0; t_row < n_chunks; t_row++) {
			current.soil_temp_c[t_row] = malloc(n_depth * sizeof(real_t));
			for (uint32_t mask_i = 0; mask_i < n_depth; mask_i++, byte_c += 4) {
				cpy_4_bytes_fn(current.soil_temp_c[t_row][mask_i].bytes,
							   bytes + byte_c);
			}
		}
		for (u_int32_t temp_i = 0; temp_i < n_chunks; temp_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.env_temp_c[temp_i].bytes,
						   (bytes + byte_c));
		}
		for (u_int32_t w_i = 0; w_i < n_chunks; w_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.water_use_ml[w_i].bytes, (bytes + byte_c));
		}

		current.pH = *(bytes + byte_c);
		SHIFT1(byte_c);
		cpy_4_bytes_fn(current.p_bar.bytes, (bytes + byte_c));
		SHIFT4(byte_c);
		cpy_4_bytes_fn(current.soil_density_kg_m3.bytes, (bytes + byte_c));
		SHIFT4(byte_c);
		cpy_2_bytes_fn(current.n_soil_adds.bytes, (bytes + byte_c));
		SHIFT2(byte_c);
		cpy_2_bytes_fn(current.n_atm_adds.bytes, (bytes + byte_c));
		SHIFT2(byte_c);

		current.soil_additives = NULL;
		current.atm_additives = NULL;

		if (current.n_soil_adds.val > 0)
			current.soil_additives = malloc(current.n_soil_adds.val
								 			* sizeof(additive_t));

		if (current.n_atm_adds.val > 0)
			current.atm_additives = malloc(current.n_atm_adds.val
										   * sizeof(additive_t));
		
		uint16_t code_idx;
		for (uint16_t j = 0, l = current.n_soil_adds.val; j < l; j++) {
			cpy_2_bytes_fn(current.soil_additives[j].code_idx.bytes,
						   (bytes + byte_c));
			code_idx = current.soil_additives[j].code_idx.val;
			current.soil_additives[j].code.val
				= adf->metadata.additive_codes[code_idx].val;
			SHIFT2(byte_c);
			cpy_4_bytes_fn(current.soil_additives[j].concentration.bytes,
						   (bytes + byte_c));
			SHIFT4(byte_c);
		}
		for (uint16_t j = 0, l = current.n_atm_adds.val; j < l; j++) {
			cpy_2_bytes_fn(current.atm_additives[j].code_idx.bytes,
						   (bytes + byte_c));
			code_idx = current.atm_additives[j].code_idx.val;
			current.atm_additives[j].code.val
				= adf->metadata.additive_codes[code_idx].val;
			SHIFT2(byte_c);
			cpy_4_bytes_fn(current.atm_additives[j].concentration.bytes,
						   (bytes + byte_c));
			SHIFT4(byte_c);
		}
		cpy_4_bytes_fn(current.repeated.bytes, (bytes + byte_c));
		SHIFT4(byte_c);

		if (current.repeated.val == 0) { return ADF_ZERO_REPEATED_SERIES; }
		
		series_crc = crc16((bytes + starting_byte), byte_c - starting_byte);
		cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
		SHIFT2(byte_c);

		if (series_crc != expected_crc.val) { return ADF_SERIES_CORRUPTED; }
		n_series += current.repeated.val - 1;
		adf->series[i] = current;

		#ifdef __ADF_DEBUG__
		printf(DEBUG_STR "Unmarshal series #%u done\n", i);
		#endif /* __ADF_DEBUG__ */
	}
	adf->metadata.n_series = n_series;
	return ADF_OK;
}

static inline bool are_reals_equal(real_t x, real_t y)
{
	return x.val < y.val ? (y.val - x.val) < EPSILON
						 : (x.val - y.val) < EPSILON;
}

bool are_additive_t_equal(additive_t x, additive_t y)
{
	return x.code.val == y.code.val
		   && are_reals_equal(x.concentration, y.concentration);
}

bool are_series_equal(const series_t *first, const series_t *second, 
					  const adf_t *adf)
{
	uint32_t n_chunks = adf->header.n_chunks.val;
	uint16_t n_wavelength = adf->header.n_wavelength.val;
	uint16_t n_depth = adf->header.n_depth.val;
	bool int_fields_eq, real_fields_eq;

	int_fields_eq = first->pH == second->pH
					&& first->n_atm_adds.val == second->n_atm_adds.val
					&& first->n_soil_adds.val == second->n_soil_adds.val;
	
	if (!int_fields_eq) return false;

	real_fields_eq = are_reals_equal(first->p_bar, second->p_bar)
					 && are_reals_equal(first->soil_density_kg_m3,
										second->soil_density_kg_m3);

	if (!real_fields_eq) return false;

	/* lastly, we need to check the arrays :( */
	for (uint32_t i = 0; i < n_chunks; i++) {
		if (!(are_reals_equal(first->env_temp_c[i], second->env_temp_c[i])
			  && are_reals_equal(first->water_use_ml[i],
								 second->water_use_ml[i])))
			return false;
	}

	for (uint32_t i = 0; i < n_chunks; i++) {
		for(uint16_t j = 0; j < n_wavelength; j++)
			if (!are_reals_equal(first->light_exposure[i][j],
				second->light_exposure[i][j]))
				return false;
	}

	for (uint32_t i = 0; i < n_chunks; i++) {
		for(uint16_t j = 0; j < n_depth; j++)
			if (!are_reals_equal(first->soil_temp_c[i][j],
				second->soil_temp_c[i][j]))
				return false;
	}

	for (uint16_t i = 0, l = first->n_soil_adds.val; i < l; i++) {
		if (!are_additive_t_equal(first->soil_additives[i],
								  second->soil_additives[i]))
			return false;
	}

	for (uint16_t i = 0, l = first->n_atm_adds.val; i < l; i++) {
		if (!are_additive_t_equal(first->atm_additives[i],
								  second->atm_additives[i]))
			return false;
	}

	return true;
}

static bool is_additive_new(uint_t *additives, uint16_t n_additives,
							 additive_t target)
{
	for (uint16_t i = 0; i < n_additives; i++) {
		if (target.code.val == additives[i].val) return false;
	}
	return true;
}

uint16_t add_series(adf_t *adf, const series_t *series_to_add)
{
	series_t *last;
	size_t new_size_series;
	additive_t *soil_add, *atm_add;
	uint16_t n_soil_add, n_atm_add, soil_addtocopy_idx, atm_addtocopy_idx,
			 res;
	uint32_t total_additives;
	cpy_2_bytes_fn = is_big_endian() 
					 ? &to_big_endian_2_bytes 
					 : &to_little_endian_2_bytes;

	if (series_to_add->repeated.val == 0) { return ADF_ZERO_REPEATED_SERIES; }

	#ifdef __ADF_DEBUG__
	printf(DEBUG_STR "Series is repeated: %u\n", series_to_add->repeated.val);
	#endif /* __ADF_DEBUG__ */

	/* Happy path, the series is repeated, just increment the counter */
	if (adf->metadata.size_series.val > 0) {
		last = adf->series + (adf->metadata.size_series.val - 1);
		if (are_series_equal(last, series_to_add, adf)) {
			last->repeated.val += series_to_add->repeated.val;
			adf->metadata.n_series += series_to_add->repeated.val;
			return ADF_OK;
		}
	}

	#ifdef __ADF_DEBUG__
	printf(DEBUG_STR "Series to add is not repeated\n");
	#endif /* __ADF_DEBUG__ */

	/* If it's not equal to the last one, and if it's not zero-repeated, 
	   then we have to add it to the series array */
	new_size_series = (adf->metadata.size_series.val + 1) * sizeof(series_t);
	adf->series = realloc(adf->series, new_size_series);
	if (!adf->series) { return ADF_RUNTIME_ERROR; }

	last = adf->series + adf->metadata.size_series.val;
	res = cpy_adf_series(last, series_to_add, adf);
	if (res != ADF_OK) { return res; }

	#ifdef __ADF_DEBUG__
	printf(DEBUG_STR "New series has been copied into series array\n");
	#endif /* __ADF_DEBUG__ */

	/* At this point, we don't know how many additives should be added to the 
	   additive_codes array in the metadata, so we allocate  */
	n_soil_add = last->n_soil_adds.val;
	n_atm_add = last->n_atm_adds.val;
	soil_addtocopy_idx = 0;
	atm_addtocopy_idx = 0;

	if (n_soil_add > 0) {
		soil_add = malloc(n_soil_add * sizeof(additive_t));
		for (uint16_t n_soil = 0; n_soil < n_soil_add; n_soil++) {
			if (is_additive_new(adf->metadata.additive_codes,
								adf->metadata.n_additives.val,
								last->soil_additives[n_soil])) {
				cpy_additive(soil_add + soil_addtocopy_idx, 
							 last->soil_additives + n_soil);
				soil_addtocopy_idx++;
			}
		}
	}

	if (n_atm_add > 0) {
		atm_add = malloc(n_atm_add * sizeof(additive_t));
		for (uint16_t n_atm = 0; n_atm < n_atm_add; n_atm++) {
			if (is_additive_new(adf->metadata.additive_codes,
								adf->metadata.n_additives.val,
								last->atm_additives[n_atm])) {
				cpy_additive(atm_add + atm_addtocopy_idx,
							 last->atm_additives + n_atm);
				atm_addtocopy_idx++;
			}
		}
	}

	uint32_t items_to_add = soil_addtocopy_idx + atm_addtocopy_idx;
	total_additives = adf->metadata.n_additives.val + items_to_add;
	if (total_additives > 0xFFFF) {
		series_free(last, adf->header.n_chunks.val);
		if (n_soil_add > 0) { free(soil_add); }
		if (n_atm_add > 0) { free(atm_add); }
		return ADF_ADDITIVE_OVERFLOW; 
	}
	size_t new_size_additives = (adf->metadata.n_additives.val + items_to_add) 
								* sizeof(uint_t);
	adf->metadata.additive_codes = realloc(adf->metadata.additive_codes, 
										   new_size_additives);

	if (!adf->metadata.additive_codes) {
		series_free(last, adf->header.n_chunks.val);
		if (n_soil_add > 0) { free(soil_add); }
		if (n_atm_add > 0) { free(atm_add); }
		return ADF_RUNTIME_ERROR; 
	}

	for (uint16_t i = adf->metadata.n_additives.val, j = 0;
		 i < adf->metadata.n_additives.val + soil_addtocopy_idx; i++, j++) {
		adf->metadata.additive_codes[i] = soil_add[j].code;
		soil_add[j].code_idx.val = i;
	}
	for (uint16_t i = adf->metadata.n_additives.val + soil_addtocopy_idx, j = 0;
		 i < adf->metadata.n_additives.val + atm_addtocopy_idx; i++, j++) {
		adf->metadata.additive_codes[i] = atm_add[j].code;
		atm_add[j].code_idx.val = i;
	}
	adf->metadata.n_additives.val += items_to_add;
	
	/* Update soil_add and atm_add of the new series to keep track of the 
	   index of the just inserted additive_code */
	for (uint16_t i = 0, l = last->n_soil_adds.val; i < l; i++) 
		last->soil_additives[i] = soil_add[i];

	for (uint16_t i = 0, l = last->n_atm_adds.val; i < l; i++)
		last->atm_additives[i] = atm_add[i];

	adf->metadata.size_series.val++;
	adf->metadata.n_series += last->repeated.val;

	if (n_soil_add > 0) { free(soil_add); }
	if (n_atm_add > 0) { free(atm_add); }
	
	return ADF_OK;
}

uint16_t remove_series(adf_t *adf)
{
	uint32_t new_size;
	series_t *last;

	if (adf->metadata.size_series.val == 0) {
		/* nothing to do here, just return an error code */
		return ADF_EMPTY_SERIES;
	}

	last = adf->series + (adf->metadata.size_series.val - 1);

	/* happy path, last series is repeated. Just decrease */
	if (last->repeated.val > 1) {
		adf->metadata.n_series--;
		last->repeated.val--;
		return ADF_OK;
	}

	adf->metadata.n_series--;
	adf->metadata.size_series.val--;
	new_size = adf->metadata.size_series.val;

	series_free(last, adf->header.n_chunks.val);

	/* just one series, not repeated */
	if (new_size == 0) {
		free(adf->series);
		adf->series = NULL;
		return ADF_OK;
	}

	adf->series = realloc(adf->series, new_size * sizeof(series_t));
	if (!adf->series) { return ADF_RUNTIME_ERROR; }

	return ADF_OK;
}

uint16_t cpy_series_starting_at(series_t *series, const adf_t *adf,
								uint32_t start_at)
{
	uint16_t res;
	uint32_t size = adf->metadata.size_series.val;

	for (uint32_t i = start_at, j = 0; i < size; i++, j++) {
		res = cpy_adf_series(series + j, adf->series + i, adf);
		if (res != ADF_OK) { return res; }
	}
	return ADF_OK;
}

uint16_t update_series(adf_t *adf, const series_t *series, uint64_t time)
{
	series_t *current, *tmp;
	uint16_t res, series_period = adf->metadata.period_sec.val;
	uint32_t new_series_size, size_series_increment;
	uint64_t l_bound_nth_series = 0, u_bound_nth_series = 0;

	for (uint32_t i = 0, l = adf->metadata.size_series.val; i < l; i++) {
		current = adf->series + i;
		l_bound_nth_series = u_bound_nth_series;
		u_bound_nth_series = l_bound_nth_series 
							 + (current->repeated.val * series_period);

		if (time > u_bound_nth_series) { continue; }

		/* if the two series are eual, nothing to do */
		if (are_series_equal(current, series, adf)) {
			adf->metadata.n_series += (series->repeated.val 
									  - current->repeated.val);
			current->repeated = series->repeated;
			return ADF_OK;
		}

		#ifdef __ADF_DEBUG__
		printf(DEBUG_STR "Series to update is not equal to the previous one\n");
		#endif /* __ADF_DEBUG__ */

		if (current->repeated.val == 1) {
			#ifdef __ADF_DEBUG__
			printf(DEBUG_STR "Repeated just one time, nothing to split\n");
			#endif /* __ADF_DEBUG__ */

			adf->metadata.n_series += (series->repeated.val 
									  - current->repeated.val);
			series_free(current, adf->header.n_chunks.val);
			cpy_adf_series(current, series, adf);
			return ADF_OK;
		}

		for (uint32_t j = 0, len = current->repeated.val; j < len; j++) {
			u_bound_nth_series = l_bound_nth_series + series_period;
			if (time < l_bound_nth_series || time >= u_bound_nth_series) {
				l_bound_nth_series += series_period; 
				u_bound_nth_series += series_period;
				continue;
			}

			tmp = malloc((adf->metadata.size_series.val - i - 1) 
						  * sizeof(series_t));
			res = cpy_series_starting_at(tmp, adf, i + 1);
			if (res != ADF_OK) { return res; }

			size_series_increment = (j == len - 1) ? 1 : 2;
			new_series_size = adf->metadata.size_series.val
							  + size_series_increment;
			adf->metadata.size_series.val = new_series_size;
			adf->series = realloc(adf->series, new_series_size
								  * sizeof(series_t));
			if (!adf->series) { return ADF_RUNTIME_ERROR; }

			res = cpy_adf_series(adf->series + (i+1), series, adf);
			if (res != ADF_OK) { return res; }
			adf->series[i].repeated.val = j;
			if (size_series_increment == 2) {
				res = cpy_adf_series(adf->series + (i+2), adf->series + i, adf);
				if (res != ADF_OK) { return res; }
				adf->series[i+2].repeated.val = 1;
			}
			for (uint32_t k1 = i + size_series_increment + 1, k2 = 0; k1 < l; k1++, k2++) {
				res = cpy_adf_series(adf->series + k1, tmp + k2, adf);
				if (res != ADF_OK) { return res; }
			}
			free(tmp);
			return ADF_OK;
		}
	}
	
	return ADF_TIME_OUT_OF_BOUND;
}

uint16_t set_series(adf_t *adf, const series_t *series, uint32_t size)
{
	uint16_t res;
	for (uint32_t i = 0; i < adf->metadata.size_series.val; i++) {
		series_free(adf->series + i, adf->header.n_chunks.val);
	}
	if (adf->metadata.size_series.val > 0)
		free(adf->series);
	
	adf->metadata.size_series.val = size;
	adf->series = malloc(size * sizeof(series_t));
	if (!adf->series) { return ADF_RUNTIME_ERROR; }
	for (uint32_t i = 0; i < adf->metadata.size_series.val; i++) {
		res = cpy_adf_series(adf->series + i, series + i, adf);
		if (res != ADF_OK) { return res; }
	}

	return ADF_OK;
}

static uint_t *get_additive_codes(pair_t *pairs, size_t size)
{
	uint_t *additives = malloc(size * sizeof(uint_t));
	for (size_t i = 0; i < size; i++) {
		additives[i].val = pairs[i].key;
		pairs[i].value = i;
	}
	return additives;
}

static uint32_t id(void *key)
{
	return *((uint32_t *) key);
}

uint16_t reindex_additives(adf_t *adf)
{
	table_t lookup_table;
	uint16_t table_code, add_idx, n_soil, n_atm;
	pair_t *additives_keys;

	if (adf->metadata.n_series == 0) {
		adf->metadata.additive_codes = NULL;
		adf->metadata.n_additives.val = 0;
		return ADF_OK;
	}

	/*
	 * Since the additive code is a unique integer id we do not need an hash 
	 * function. Hence, the `id` function just return the 4-byte additive code
	 */
	if ((table_code = table_init(&lookup_table, 1024, 1024, &id)) != LM_OK)
		return ADF_RUNTIME_ERROR;

	for (uint32_t i = 0, l = adf->metadata.size_series.val; i < l; i++) {
		n_soil = adf->series[i].n_soil_adds.val;
		n_atm = adf->series[i].n_atm_adds.val;
		for (uint16_t j = 0; j < n_soil; j++) {
			table_code = table_put(&lookup_table, 
								   adf->series[i].soil_additives[j].code.val,
								   j);
			if (table_code == LM_MAP_SIZE_OVERFLOW)
				return ADF_ADDITIVE_OVERFLOW;
			if (table_code != LM_OK)
				return ADF_RUNTIME_ERROR; 
		}
	
		for (uint16_t j = 0; j < n_atm; j++) {
			table_code = table_put(&lookup_table,
								   adf->series[i].atm_additives[j].code.val,
								   j);
			if (table_code == LM_MAP_SIZE_OVERFLOW)
				return ADF_ADDITIVE_OVERFLOW;
			if (table_code != LM_OK)
				return ADF_RUNTIME_ERROR; 
		}
	}

	additives_keys = malloc(lookup_table.size * sizeof(pair_t));
	if (table_get_pairs(&lookup_table, additives_keys) != LM_OK) {
		return ADF_RUNTIME_ERROR; 
	}

	adf->metadata.additive_codes = get_additive_codes(additives_keys, 
													  lookup_table.size);
	adf->metadata.n_additives.val = (uint16_t)lookup_table.size;
	
	for (uint32_t i = 0, l = adf->metadata.size_series.val; i < l; i++) {
		n_soil = adf->series[i].n_soil_adds.val;
		n_atm = adf->series[i].n_atm_adds.val;
		additive_t *current;
		for (uint16_t j = 0; j < n_soil; j++) {
			current = (adf->series[i].soil_additives + j);
			add_idx = (uint16_t) table_get(&lookup_table, 
										   current->code.val);
			current->code_idx.val = add_idx;
		}
		for (uint16_t j = 0; j < n_atm; j++) {
			current = (adf->series[i].atm_additives + j);
			add_idx = (uint16_t) table_get(&lookup_table, 
										   current->code.val);
			current->code_idx.val = add_idx;
		}
	}

	table_free(&lookup_table);
	free(additives_keys);

	return ADF_OK;
}

adf_header_t create_header(uint8_t farming_tec, uint32_t n_chunks,
						   uint32_t min_w_len_nm, uint32_t max_w_len_nm,
						   uint32_t n_wavelrngth)
{
	return (adf_header_t) {
	    .signature = { __ADF_SIGNATURE__ },
	    .version = { __ADF_VERSION__ },
	    .farming_tec = farming_tec,
	    .max_w_len_nm = { max_w_len_nm },
	    .min_w_len_nm = { min_w_len_nm },
	    .n_wavelength = { n_wavelrngth },
	    .n_chunks = { n_chunks }
	};
}

void  metadata_init(adf_meta_t *metadata, uint32_t period_sec)
{
	metadata->additive_codes = NULL;
	metadata->n_additives.val = 0;
	metadata->size_series.val = 0;
	metadata->period_sec.val = period_sec;
	metadata->n_series = 0;
}

void adf_init(adf_t *adf, adf_header_t header, uint32_t period_sec)
{
	adf_meta_t metadata;
	metadata_init(&metadata, period_sec);
	adf->header = header;
	adf->metadata = metadata;
	adf->series = NULL;
}

adf_t create_empty_adf(adf_header_t header, uint16_t period_sec)
{
	adf_t adf;
	adf_init(&adf, header, period_sec);
	return adf;
}

uint16_t init_empty_series(series_t *series, uint32_t n_chunks,
						   uint16_t n_wavelenght, uint16_t n_depth,
						   uint16_t n_soil_additives, uint16_t n_atm_additives)
{
	series->n_soil_adds.val = n_soil_additives;
	series->n_atm_adds.val = n_atm_additives;
	series->env_temp_c = malloc(n_chunks * sizeof(real_t));
	series->water_use_ml = malloc(n_chunks * sizeof(real_t));
	series->soil_additives = malloc(n_soil_additives
									* sizeof(additive_t));
	series->atm_additives = malloc(n_atm_additives
								   * sizeof(additive_t));
	series->light_exposure = malloc(n_chunks * sizeof(real_t *));
	series->soil_temp_c = malloc(n_chunks * sizeof(real_t *));

	for (uint32_t i = 0; i < n_chunks; i++) {
		series->light_exposure[i] = malloc(n_wavelenght * sizeof(real_t));
		series->soil_temp_c[i] = malloc(n_depth * sizeof(real_t));
	}

	if (!series->env_temp_c  
		|| !series->water_use_ml  
		|| !series->light_exposure
		|| !series->soil_additives
		|| !series->atm_additives)
		return ADF_RUNTIME_ERROR;
	
	return ADF_OK;
}

void metadata_free(adf_meta_t *metadata)
{
	free(metadata->additive_codes);
	metadata->additive_codes = NULL;
}

void series_free(series_t *series, uint32_t n_chunks)
{ 
	for (uint32_t i = 0; i < n_chunks; i++) {
		free(series->light_exposure[i]);
		free(series->soil_temp_c[i]);
	}
	free(series->light_exposure);
	free(series->soil_temp_c);
	free(series->env_temp_c);
	free(series->water_use_ml);
	if (series->n_soil_adds.val > 0) { free(series->soil_additives); }
	if (series->n_atm_adds.val > 0) { free(series->atm_additives); }

	series->light_exposure = NULL;
	series->env_temp_c = NULL;
	series->water_use_ml = NULL;
	series->soil_additives = NULL;
	series->atm_additives = NULL;
}

void adf_free(adf_t *adf)
{
	adf_meta_t *metadata = (adf_meta_t *) &(adf->metadata);
	metadata_free(metadata);
	for (uint32_t i = 0, l = adf->metadata.size_series.val; i < l; i++) 
		series_free(adf->series + i, adf->header.n_chunks.val);
	free(adf->series);
	adf->series = NULL;
}

uint16_t cpy_additive(additive_t *target, const additive_t *source)
{
	if (!source) { return ADF_NULL_ADDITIVE_SOURCE; }
	if (!target) { return ADF_NULL_ADDITIVE_TARGET; }

	target->code = source->code;
	target->code_idx = source->code_idx;
	target->concentration = source->concentration;

	return ADF_OK;
}

uint16_t cpy_adf_series(series_t *target, const series_t *source,
						const adf_t *adf)
{
	uint32_t n_chunks;
	uint16_t res, n_wavelength, n_depth;

	if (!source) { return ADF_NULL_SERIES_SOURCE; }
	if (!target) { return ADF_NULL_SERIES_TARGET; }

	n_chunks = adf->header.n_chunks.val;
	n_wavelength = adf->header.n_wavelength.val;
	n_depth = adf->header.n_depth.val;
	target->n_atm_adds = source->n_atm_adds;
	target->n_soil_adds = source->n_soil_adds;
	target->p_bar = source->p_bar;
	target->pH = source->pH;
	target->repeated = source->repeated;
	target->soil_density_kg_m3 = source->soil_density_kg_m3;
	target->water_use_ml = malloc(n_chunks * sizeof(real_t));
	target->env_temp_c = malloc(n_chunks * sizeof(real_t));
	target->light_exposure = malloc(n_chunks * sizeof(real_t *));
	target->soil_temp_c = malloc(n_chunks * sizeof(real_t *));
	target->soil_additives = NULL;
	target->atm_additives = NULL;

	for (uint32_t i = 0; i < n_chunks; i++) {
		target->water_use_ml[i] = source->water_use_ml[i];
		target->env_temp_c[i] = source->env_temp_c[i];
	}
	for (uint32_t i = 0; i < n_chunks; i++) {
		target->light_exposure[i] = malloc(n_wavelength * sizeof(real_t));
		for (uint16_t j = 0; j < n_wavelength; j++) {
			target->light_exposure[i][j] = source->light_exposure[i][j];
		}
	}
	for (uint32_t i = 0; i < n_chunks; i++) {
		target->soil_temp_c[i] = malloc(n_depth * sizeof(real_t));
		for (uint16_t j = 0; j < n_depth; j++) {
			target->soil_temp_c[i][j] = source->soil_temp_c[i][j];
		}
	}

	if (source->n_soil_adds.val > 0) {
		target->soil_additives = malloc(target->n_soil_adds.val
										* sizeof(additive_t));
		for (uint16_t i = 0, l = target->n_soil_adds.val; i < l; i++) {
			res = cpy_additive(target->soil_additives + i, 
							   source->soil_additives + i);
			if (res != ADF_OK) { return res; }
		}
	}

	if (source->n_atm_adds.val > 0) {
		target->atm_additives = malloc(target->n_atm_adds.val
									   * sizeof(additive_t));
		for (uint16_t i = 0, l = target->n_atm_adds.val; i < l; i++) {
			res = cpy_additive(target->atm_additives + i, 
							   source->atm_additives + i);
			if (res != ADF_OK) { return res; }
		}
	}

	return ADF_OK;
}

uint16_t cpy_adf_metadata(adf_meta_t *target, const adf_meta_t *source)
{
	if (!source) { return ADF_NULL_META_SOURCE; }
	if (!target) { return ADF_NULL_META_TARGET; }

	*target = *source;
	target->additive_codes = malloc(target->n_additives.val * sizeof(uint_t));

	for (uint16_t i = 0, l = target->n_additives.val; i < l; i++) 
		target->additive_codes[i] = source->additive_codes[i];

	return ADF_OK;
}

uint16_t cpy_adf_header(adf_header_t *target, const adf_header_t *source)
{
	if (!source) { return ADF_NULL_HEADER_SOURCE; }
	if (!target) { return ADF_NULL_HEADER_TARGET; }

	*target = *source;
	return ADF_OK;
}

uint16_t cpy_adf(adf_t *target, const adf_t *source)
{
	uint16_t res;
	uint32_t size_series;
	
	if (!source) { return ADF_NULL_SOURCE; }
	if (!target) { return ADF_NULL_TARGET; }

	res = cpy_adf_header(&target->header, &source->header);
	if (res != ADF_OK) { return res; }

	res = cpy_adf_metadata(&target->metadata, &source->metadata);
	if (res != ADF_OK) { return res; }

	size_series = source->metadata.size_series.val;
	if (size_series > 0) {
		target->series = malloc(size_series * sizeof(series_t));
		for (uint32_t i = 0, l = target->metadata.size_series.val; i < l; i++) {
			res = cpy_adf_series(target->series + i, source->series + i,
								source);
			if(res != ADF_OK) { return res; }
		}
	}
	else {
		target->series = NULL;
	}
	
	return ADF_OK;
}
