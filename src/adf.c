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
#include "crc.h"
#include <stdlib.h>

#define SHIFT_COUNTER(n) (byte_c += n)

typedef void (*number_bytes_copy)(uint8_t *, const uint8_t *);

static number_bytes_copy cpy_4_bytes_fn;
static number_bytes_copy cpy_2_bytes_fn;

static _Bool is_big_endian()
{
	union {
		uint32_t val;
		uint8_t bytes[4];
	} endianess = {0x01000000};

	return endianess.bytes[0];
}

static void to_big_endian_4_bytes(uint8_t *dest, const uint8_t *source)
{
	*(dest)		= *source;
	*(dest + 1) = *(source + 1);
	*(dest + 2) = *(source + 2);
	*(dest + 3) = *(source + 3);
}

static void to_little_endian_4_bytes(uint8_t *dest, const uint8_t *source)
{
	*(dest)		= *(source + 3);
	*(dest + 1) = *(source + 2);
	*(dest + 2) = *(source + 1);
	*(dest + 3) = *source;
}

static void to_big_endian_2_bytes(uint8_t *dest, const uint8_t *source)
{
	*(dest)		= *source;
	*(dest + 1) = *(source + 1);
}

static void to_little_endian_2_bytes(uint8_t *dest, const uint8_t *source)
{
	*(dest)		= *(source + 1);
	*(dest + 1) = *source;
}

unsigned get_version(void) { return __ADF_VERSION__; }

size_t size_series_t(uint32_t n_chunks, series_t series)
{
	return (n_chunks * 4) +				  /* light_exposure */
		   (n_chunks * 4) +				  /* temp_celsius */
		   (n_chunks * 4) +				  /* water_use_ml */
		   1 +							  /* pH */
		   4 +							  /* p_bar */
		   4 +							  /* soil_density_t_m3 */
		   2 +							  /* n_soil_adds */
		   2 +							  /* n_atm_adds */
		   (6 * series.n_soil_adds.val) + /* soil_additives */
		   (6 * series.n_soil_adds.val) + /* atm_additives */
		   4 +							  /* repeated */
		   2;							  /* crc */
}

size_t size_medatata_t(adf_meta_t metadata)
{
	return 4 +								/* n_series */
		   4 +								/* period_sec */
		   2 +								/* n_additives */
		   (metadata.n_additives.val * 4) + /* additive_codes */
		   2;								/* crc */
}

size_t size_header(void)
{
	return 4 + /* signature */
		   1 + /* version */
		   1 + /* farming_tec */
		   4 + /* n_wavelength */
		   4 + /* min_w_len_nm */
		   4 + /* max_w_len_nm */
		   4 + /* n_chunks */
		   2;  /* crc */
}

size_t size_adf_t(adf_t data)
{
	const size_t head_metadata_size = size_header() + size_medatata_t(data.metadata);
	size_t series_size				= 0;
	for (uint32_t i = 0, l = data.metadata.n_series.val; i < l; i++) {
		series_size += size_series_t(data.header.n_chunks.val, data.series[i]);
	}
	return head_metadata_size + series_size;
}

uint8_t *bytes_alloc(adf_t data)
{
	return (uint8_t *)malloc(size_adf_t(data));
}

static int get_add_code_idx(uint_t *codes, uint_small_t n_codes, uint_t code_idx)
{
	for (uint16_t i = 0; i < n_codes.val; i++) {
		if ((codes + i)->val == code_idx.val)
			return i;
	}
	return -1;
}

int marshal(uint8_t *bytes, adf_t data)
{
	size_t byte_c			= 0;
	uint_small_t crc_16bits = {0xFFFF};
	cpy_4_bytes_fn			= is_big_endian()
								  ? &to_big_endian_4_bytes
								  : &to_little_endian_4_bytes;
	cpy_2_bytes_fn			= is_big_endian()
								  ? &to_big_endian_2_bytes
								  : &to_little_endian_2_bytes;
	if (!bytes)
		return RUNTIME_ERROR;
	cpy_4_bytes_fn((bytes + byte_c), data.header.signature.bytes);
	SHIFT_COUNTER(4);
	*(bytes + byte_c) = data.header.version;
	SHIFT_COUNTER(1);
	*(bytes + byte_c) = data.header.farming_tec;
	SHIFT_COUNTER(1);
	cpy_4_bytes_fn((bytes + byte_c), data.header.n_wavelength.bytes);
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn((bytes + byte_c), data.header.min_w_len_nm.bytes);
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn((bytes + byte_c), data.header.max_w_len_nm.bytes);
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn((bytes + byte_c), data.header.n_chunks.bytes);
	SHIFT_COUNTER(4);
	crc_16bits.val = crc16(bytes, byte_c);
	cpy_2_bytes_fn((bytes + byte_c), crc_16bits.bytes);
	SHIFT_COUNTER(2);
	cpy_4_bytes_fn((bytes + byte_c), data.metadata.n_series.bytes);
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn((bytes + byte_c), data.metadata.period_sec.bytes);
	SHIFT_COUNTER(4);
	cpy_2_bytes_fn((bytes + byte_c), data.metadata.n_additives.bytes);
	SHIFT_COUNTER(2);

	for (uint16_t i = 0, l = data.metadata.n_additives.val; i < l; i++, byte_c += 4) {
		cpy_4_bytes_fn((bytes + byte_c), data.metadata.additive_codes[i].bytes);
	}

	crc_16bits.val = crc16((bytes + size_header()), byte_c - size_header());
	cpy_2_bytes_fn((bytes + byte_c), crc_16bits.bytes);
	SHIFT_COUNTER(2);

	for (uint32_t i = 0, n_iter = data.metadata.n_series.val; i < n_iter; i++) {
		series_t current	 = data.series[i];
		size_t starting_byte = byte_c;
		if (!current.light_exposure)
			return RUNTIME_ERROR;
		for (uint32_t mask_i = 0; mask_i < data.header.n_chunks.val; mask_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c), current.light_exposure[mask_i].bytes);
		}
		if (!current.temp_celsius)
			return RUNTIME_ERROR;
		for (uint32_t temp_i = 0; temp_i < data.header.n_chunks.val; temp_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c), current.temp_celsius[temp_i].bytes);
		}
		if (!current.water_use_ml)
			return RUNTIME_ERROR;
		for (uint32_t w_i = 0; w_i < data.header.n_chunks.val; w_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c), current.water_use_ml[w_i].bytes);
		}
		*(bytes + byte_c) = current.pH;
		SHIFT_COUNTER(1);
		cpy_4_bytes_fn((bytes + byte_c), current.p_bar.bytes);
		SHIFT_COUNTER(4);
		cpy_4_bytes_fn((bytes + byte_c), current.soil_density_kg_m3.bytes);
		SHIFT_COUNTER(4);
		cpy_2_bytes_fn((bytes + byte_c), current.n_soil_adds.bytes);
		SHIFT_COUNTER(2);
		cpy_2_bytes_fn((bytes + byte_c), current.n_atm_adds.bytes);
		SHIFT_COUNTER(2);
		for (uint16_t j = 0, l = current.n_soil_adds.val; j < l; j++) {
			cpy_2_bytes_fn((bytes + byte_c), current.soil_additives[j].code_idx.bytes);
			SHIFT_COUNTER(2);
			cpy_4_bytes_fn((bytes + byte_c), current.soil_additives[j].concentration.bytes);
			SHIFT_COUNTER(4);
		}
		for (uint16_t j = 0, l = current.n_atm_adds.val; j < l; j++) {
			cpy_2_bytes_fn((bytes + byte_c), current.atm_additives[j].code_idx.bytes);
			SHIFT_COUNTER(2);
			cpy_4_bytes_fn((bytes + byte_c), current.atm_additives[j].concentration.bytes);
			SHIFT_COUNTER(4);
		}
		cpy_4_bytes_fn((bytes + byte_c), current.repeated.bytes);
		SHIFT_COUNTER(4);

		crc_16bits.val = crc16((bytes + starting_byte), byte_c - starting_byte);
		cpy_2_bytes_fn((bytes + byte_c), crc_16bits.bytes);
		SHIFT_COUNTER(2);
	}
	return OK;
}

int unmarshal(adf_t *adf, const uint8_t *bytes)
{
	size_t byte_c = 0;
	uint_small_t expected_crc;
	cpy_4_bytes_fn = is_big_endian()
						 ? &to_big_endian_4_bytes
						 : &to_little_endian_4_bytes;
	cpy_2_bytes_fn = is_big_endian()
						 ? &to_big_endian_2_bytes
						 : &to_little_endian_2_bytes;

	if (!bytes || !adf)
		return RUNTIME_ERROR;

	cpy_4_bytes_fn(adf->header.signature.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	adf->header.version = *(bytes + byte_c);
	SHIFT_COUNTER(1);
	adf->header.farming_tec = *(bytes + byte_c);
	SHIFT_COUNTER(1);
	cpy_4_bytes_fn(adf->header.n_wavelength.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn(adf->header.min_w_len_nm.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn(adf->header.max_w_len_nm.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn(adf->header.n_chunks.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	uint16_t header_crc = crc16(bytes, byte_c);
	cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);

	if (header_crc != expected_crc.val)
		return HEADER_CORRUPTED;

	cpy_4_bytes_fn(adf->metadata.n_series.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_4_bytes_fn(adf->metadata.period_sec.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_2_bytes_fn(adf->metadata.n_additives.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);

	if (!(adf->metadata.additive_codes = malloc(adf->metadata.n_additives.val * sizeof(uint_t))))
		return RUNTIME_ERROR;

	for (uint16_t i = 0, l = adf->metadata.n_additives.val; i < l; i++, byte_c += 4) {
		cpy_4_bytes_fn(adf->metadata.additive_codes[i].bytes, (bytes + byte_c));
	}

	uint16_t meta_crc = crc16((bytes + size_header()), byte_c - size_header());
	cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);

	if (meta_crc != expected_crc.val)
		return METADATA_CORRUPTED;

	if (!(adf->series = malloc(adf->metadata.n_series.val * sizeof(series_t))))
		return RUNTIME_ERROR;

	for (uint32_t i = 0, n_iter = adf->metadata.n_series.val; i < n_iter; i++) {
		series_t current;
		size_t starting_byte = byte_c;
		if (!(current.light_exposure = malloc(adf->header.n_chunks.val * sizeof(real_t))))
			return RUNTIME_ERROR;

		if (!(current.temp_celsius = malloc(adf->header.n_chunks.val * sizeof(real_t))))
			return RUNTIME_ERROR;

		if (!(current.water_use_ml = malloc(adf->header.n_chunks.val * sizeof(real_t))))
			return RUNTIME_ERROR;

		for (u_int32_t mask_i = 0; mask_i < adf->header.n_chunks.val; mask_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.light_exposure[mask_i].bytes, (bytes + byte_c));
		}
		for (u_int32_t temp_i = 0; temp_i < adf->header.n_chunks.val; temp_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.temp_celsius[temp_i].bytes, (bytes + byte_c));
		}
		for (u_int32_t w_i = 0; w_i < adf->header.n_chunks.val; w_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.water_use_ml[w_i].bytes, (bytes + byte_c));
		}

		current.pH = *(bytes + byte_c);
		SHIFT_COUNTER(1);
		cpy_4_bytes_fn(current.p_bar.bytes, (bytes + byte_c));
		SHIFT_COUNTER(4);
		cpy_4_bytes_fn(current.soil_density_kg_m3.bytes, (bytes + byte_c));
		SHIFT_COUNTER(4);
		cpy_2_bytes_fn(current.n_soil_adds.bytes, (bytes + byte_c));
		SHIFT_COUNTER(2);
		cpy_2_bytes_fn(current.n_atm_adds.bytes, (bytes + byte_c));
		SHIFT_COUNTER(2);

		if (!(current.soil_additives = malloc(current.n_soil_adds.val * sizeof(additive_t))))
			return RUNTIME_ERROR;

		if (!(current.atm_additives = malloc(current.n_atm_adds.val * sizeof(additive_t))))
			return RUNTIME_ERROR;

		uint16_t code_idx;
		for (uint16_t j = 0, l = current.n_soil_adds.val; j < l; j++) {
			cpy_2_bytes_fn(current.soil_additives[j].code_idx.bytes, (bytes + byte_c));
			code_idx						   = current.soil_additives[j].code_idx.val;
			current.soil_additives[j].code.val = adf->metadata.additive_codes[code_idx].val;
			SHIFT_COUNTER(2);
			cpy_4_bytes_fn(current.soil_additives[j].concentration.bytes, (bytes + byte_c));
			SHIFT_COUNTER(4);
		}
		for (uint16_t j = 0, l = current.n_atm_adds.val; j < l; j++) {
			cpy_2_bytes_fn(current.atm_additives[j].code_idx.bytes, (bytes + byte_c));
			code_idx						  = current.atm_additives[j].code_idx.val;
			current.atm_additives[j].code.val = adf->metadata.additive_codes[code_idx].val;
			SHIFT_COUNTER(2);
			cpy_4_bytes_fn(current.atm_additives[j].concentration.bytes, (bytes + byte_c));
			SHIFT_COUNTER(4);
		}
		cpy_4_bytes_fn(current.repeated.bytes, (bytes + byte_c));
		SHIFT_COUNTER(4);

		uint16_t series_crc = crc16((bytes + starting_byte), byte_c - starting_byte);
		cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
		SHIFT_COUNTER(2);

		if (series_crc != expected_crc.val)
			return SERIES_CORRUPTED;

		adf->series[i] = current;
	}
	return OK;
}

typedef struct {
	series_t *series;
	size_t *repeated_values;
} series_list_t;

static series_list_t init_series_list(series_t *series, size_t n_series)
{
	size_t *r_vals = malloc(n_series * sizeof(size_t));
	for (size_t i = 0; i < n_series; i++)
		r_vals[i] = 0;

	return (series_list_t){
		.series			 = series,
		.repeated_values = r_vals
	};
}

// static long add_series(adf_t *adf, series_t series)
// {
// 	const uint32_t last_series_i = adf->metadata.n_series.val - 1;
// 	series_t *current = (adf->series + last_series_i);
// 	if (current->crc.val == series.crc.val) {
// 		current->repeated.val++;
// 	}
// 	else {
// 	}
// 	return 0;
// }

static series_list_t series_to_add(series_t *series, size_t n_series)
{
	series_list_t series_to_add = init_series_list(series, n_series);
	size_t index				= 0ul,
		   successor			= index + 1,
		   count				= 0ul;

	do {
		if (series_to_add.series[index].crc.val == series_to_add.series[successor].crc.val) {
			series_to_add.repeated_values[index]++;
			successor++;
		}
		else {
			series_to_add.repeated_values[index] = 1;
			index++;
			successor++;
		}
	} while (successor < n_series);

	return series_to_add;
}

int add_series_batch(adf_t *adf, series_t *series, size_t n_series)
{
	// size_t unique_series_size = series_to_add(series, n_series);
	// uint32_t start_series	  = adf->metadata.n_series.val;

	// if (!realloc(adf->series, unique_series_size))
	// 	return RUNTIME_ERROR;
	// for (size_t i = 0; i < n_series; i++, start_series++) {
	// 	adf->series[start_series] = *(series + i);
	// }

	return 0;
}

int add_series(adf_t *adf, series_t series)
{
	series_t *last = adf->series + (adf->metadata.n_series.val - 1);
	cpy_2_bytes_fn = is_big_endian()
						 ? &to_big_endian_2_bytes
						 : &to_little_endian_2_bytes;

	// Sistemare equivalenza...
	// fare a meno di crc
	if (last->crc.val == series.crc.val) {
		last->repeated.val++;
		return OK;
	}

	if (!realloc(adf->series, adf->metadata.n_series.val + 1)) {
		return RUNTIME_ERROR;
	}

	uint32_t avg_series_sec = adf->metadata.period_sec.val / adf->metadata.n_series.val;
	adf->metadata.period_sec.val += avg_series_sec;

	for (uint16_t i = 0; i < adf->metadata.n_additives.val; i++) {
		uint32_t add_code = adf->metadata.additive_codes[i].val;

		for (uint16_t n_soil = 0; n_soil < adf->metadata.n_additives.val; n_soil++) {
			if (series.soil_additives[n_soil].code.val != add_code) {
				adf->metadata.n_additives.val++;
				if (!realloc(adf->metadata.additive_codes, adf->metadata.n_additives.val))
					return RUNTIME_ERROR;
				adf->metadata.additive_codes[adf->metadata.n_additives.val - 1] = series.soil_additives[n_soil].code;
				cpy_2_bytes_fn(series.soil_additives[n_soil].code_idx.bytes, adf->metadata.n_additives.bytes);
			}
		}

		for (uint16_t n_atm = 0; n_atm < adf->metadata.n_additives.val; n_atm++) {
			if (series.atm_additives[n_atm].code.val != add_code) {
				adf->metadata.n_additives.val++;
				if (!realloc(adf->metadata.additive_codes, adf->metadata.n_additives.val))
					return RUNTIME_ERROR;
				adf->metadata.additive_codes[adf->metadata.n_additives.val - 1] = series.atm_additives[n_atm].code;
				cpy_2_bytes_fn(series.atm_additives[n_atm].code_idx.bytes, adf->metadata.n_additives.bytes);
			}
		}
	}
	adf->series[++(adf->metadata.n_series.val)] = series;

	return OK;
}
