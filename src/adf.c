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
 * Terius is distributed in the hope that it will be useful,
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
	} endianess = { 0x01000000 };

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

size_t size_series_t(uint32_t n_chunks, series_t series)
{
	return (n_chunks * 4)				  /* light_exposure */
		   + (n_chunks * 4)				  /* temp_celsius */
		   + (n_chunks * 4)				  /* water_use_ml */
		   + 1							  /* pH */
		   + 4							  /* p_bar */
		   + 4							  /* soil_density_t_m3 */
		   + 2							  /* n_soil_adds */
		   + 2							  /* n_atm_adds */
		   + (6 * series.n_soil_adds.val) /* soil_additives */
		   + (6 * series.n_soil_adds.val) /* atm_additives */
		   + 4							  /* repeated */
		   + 2;							  /* crc */
}

size_t size_medatata_t(adf_meta_t metadata)
{
	return 4								/* size_series */
		   + 2								/* period_sec */
		   + 2								/* n_additives */
		   + (metadata.n_additives.val * 4) /* additive_codes */
		   + 2;								/* crc */
}

size_t size_header(void)
{
	return 4	/* signature */
		   + 1	/* version */
		   + 1	/* farming_tec */
		   + 4	/* n_wavelength */
		   + 4	/* min_w_len_nm */
		   + 4	/* max_w_len_nm */
		   + 4	/* n_chunks */
		   + 2; /* crc */
}

size_t size_adf_t(adf_t data)
{
	const size_t head_metadata_size
		= size_header() + size_medatata_t(data.metadata);
	size_t series_size = 0;
	for (uint32_t i = 0, l = data.metadata.size_series.val; i < l; i++) {
		series_size += size_series_t(data.header.n_chunks.val, data.series[i]);
	}
	return head_metadata_size + series_size;
}

uint8_t *bytes_alloc(adf_t data) { return (uint8_t *)malloc(size_adf_t(data)); }

int marshal(uint8_t *bytes, adf_t data)
{
	size_t byte_c = 0;
	uint_small_t crc_16bits;
	cpy_4_bytes_fn
		= is_big_endian() ? &to_big_endian_4_bytes : &to_little_endian_4_bytes;
	cpy_2_bytes_fn
		= is_big_endian() ? &to_big_endian_2_bytes : &to_little_endian_2_bytes;
	if (!bytes) return RUNTIME_ERROR;
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
	cpy_4_bytes_fn((bytes + byte_c), data.metadata.size_series.bytes);
	SHIFT_COUNTER(4);
	cpy_2_bytes_fn((bytes + byte_c), data.metadata.period_sec.bytes);
	SHIFT_COUNTER(2);
	cpy_2_bytes_fn((bytes + byte_c), data.metadata.n_additives.bytes);
	SHIFT_COUNTER(2);

	for (uint16_t i = 0, l = data.metadata.n_additives.val; i < l;
		 i++, byte_c += 4) {
		cpy_4_bytes_fn((bytes + byte_c), data.metadata.additive_codes[i].bytes);
	}

	crc_16bits.val = crc16((bytes + size_header()), byte_c - size_header());
	cpy_2_bytes_fn((bytes + byte_c), crc_16bits.bytes);
	SHIFT_COUNTER(2);

	for (uint32_t i = 0, n_iter = data.metadata.size_series.val; i < n_iter;
		 i++) {
		series_t current = data.series[i];
		size_t starting_byte = byte_c;
		if (!current.light_exposure) return RUNTIME_ERROR;
		for (uint32_t mask_i = 0; mask_i < data.header.n_chunks.val;
			 mask_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c),
						   current.light_exposure[mask_i].bytes);
		}
		if (!current.temp_celsius) return RUNTIME_ERROR;
		for (uint32_t temp_i = 0; temp_i < data.header.n_chunks.val;
			 temp_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c),
						   current.temp_celsius[temp_i].bytes);
		}
		if (!current.water_use_ml) return RUNTIME_ERROR;
		for (uint32_t w_i = 0; w_i < data.header.n_chunks.val;
			 w_i++, byte_c += 4) {
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
			cpy_2_bytes_fn((bytes + byte_c),
						   current.soil_additives[j].code_idx.bytes);
			SHIFT_COUNTER(2);
			cpy_4_bytes_fn((bytes + byte_c),
						   current.soil_additives[j].concentration.bytes);
			SHIFT_COUNTER(4);
		}
		for (uint16_t j = 0, l = current.n_atm_adds.val; j < l; j++) {
			cpy_2_bytes_fn((bytes + byte_c),
						   current.atm_additives[j].code_idx.bytes);
			SHIFT_COUNTER(2);
			cpy_4_bytes_fn((bytes + byte_c),
						   current.atm_additives[j].concentration.bytes);
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
	cpy_4_bytes_fn
		= is_big_endian() ? &to_big_endian_4_bytes : &to_little_endian_4_bytes;
	cpy_2_bytes_fn
		= is_big_endian() ? &to_big_endian_2_bytes : &to_little_endian_2_bytes;

	if (!bytes || !adf) return RUNTIME_ERROR;

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

	if (header_crc != expected_crc.val) return HEADER_CORRUPTED;

	cpy_4_bytes_fn(adf->metadata.size_series.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_2_bytes_fn(adf->metadata.period_sec.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);
	cpy_2_bytes_fn(adf->metadata.n_additives.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);

	if (!(adf->metadata.additive_codes
		  = malloc(adf->metadata.n_additives.val * sizeof(uint_t))))
		return RUNTIME_ERROR;

	for (uint16_t i = 0, l = adf->metadata.n_additives.val; i < l;
		 i++, byte_c += 4) {
		cpy_4_bytes_fn(adf->metadata.additive_codes[i].bytes, (bytes + byte_c));
	}

	uint16_t meta_crc = crc16((bytes + size_header()), byte_c - size_header());
	cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);

	if (meta_crc != expected_crc.val) return METADATA_CORRUPTED;

	if (!(adf->series
		  = malloc(adf->metadata.size_series.val * sizeof(series_t))))
		return RUNTIME_ERROR;

	for (uint32_t i = 0, n_iter = adf->metadata.size_series.val; i < n_iter;
		 i++) {
		series_t current;
		size_t starting_byte = byte_c;
		if (!(current.light_exposure
			  = malloc(adf->header.n_chunks.val * sizeof(real_t))))
			return RUNTIME_ERROR;

		if (!(current.temp_celsius
			  = malloc(adf->header.n_chunks.val * sizeof(real_t))))
			return RUNTIME_ERROR;

		if (!(current.water_use_ml
			  = malloc(adf->header.n_chunks.val * sizeof(real_t))))
			return RUNTIME_ERROR;

		for (u_int32_t mask_i = 0; mask_i < adf->header.n_chunks.val;
			 mask_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.light_exposure[mask_i].bytes,
						   (bytes + byte_c));
		}
		for (u_int32_t temp_i = 0; temp_i < adf->header.n_chunks.val;
			 temp_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.temp_celsius[temp_i].bytes,
						   (bytes + byte_c));
		}
		for (u_int32_t w_i = 0; w_i < adf->header.n_chunks.val;
			 w_i++, byte_c += 4) {
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

		if (!(current.soil_additives
			  = malloc(current.n_soil_adds.val * sizeof(additive_t))))
			return RUNTIME_ERROR;

		if (!(current.atm_additives
			  = malloc(current.n_atm_adds.val * sizeof(additive_t))))
			return RUNTIME_ERROR;

		uint16_t code_idx;
		for (uint16_t j = 0, l = current.n_soil_adds.val; j < l; j++) {
			cpy_2_bytes_fn(current.soil_additives[j].code_idx.bytes,
						   (bytes + byte_c));
			code_idx = current.soil_additives[j].code_idx.val;
			current.soil_additives[j].code.val
				= adf->metadata.additive_codes[code_idx].val;
			SHIFT_COUNTER(2);
			cpy_4_bytes_fn(current.soil_additives[j].concentration.bytes,
						   (bytes + byte_c));
			SHIFT_COUNTER(4);
		}
		for (uint16_t j = 0, l = current.n_atm_adds.val; j < l; j++) {
			cpy_2_bytes_fn(current.atm_additives[j].code_idx.bytes,
						   (bytes + byte_c));
			code_idx = current.atm_additives[j].code_idx.val;
			current.atm_additives[j].code.val
				= adf->metadata.additive_codes[code_idx].val;
			SHIFT_COUNTER(2);
			cpy_4_bytes_fn(current.atm_additives[j].concentration.bytes,
						   (bytes + byte_c));
			SHIFT_COUNTER(4);
		}
		cpy_4_bytes_fn(current.repeated.bytes, (bytes + byte_c));
		SHIFT_COUNTER(4);

		uint16_t series_crc
			= crc16((bytes + starting_byte), byte_c - starting_byte);
		cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
		SHIFT_COUNTER(2);

		if (series_crc != expected_crc.val) return SERIES_CORRUPTED;

		adf->series[i] = current;
	}
	return OK;
}

static inline _Bool are_reals_equal(real_t x, real_t y)
{
	return x.val < y.val ? (y.val - x.val) < EPSILON
						 : (x.val - y.val) < EPSILON;
}

static _Bool are_additive_t_equal(additive_t x, additive_t y)
{
	return x.code.val == y.code.val && x.code_idx.val == y.code_idx.val
		   && are_reals_equal(x.concentration, y.concentration);
}

_Bool are_series_equal(series_t first, series_t second, uint32_t n_chunks)
{
	_Bool int_fields_eq = first.pH == second.pH
						  && first.n_atm_adds.val == second.n_atm_adds.val
						  && first.n_soil_adds.val == second.n_soil_adds.val;

	if (!int_fields_eq) return false;

	_Bool real_fields_eq = are_reals_equal(first.p_bar, second.p_bar)
						   && are_reals_equal(first.soil_density_kg_m3,
											  second.soil_density_kg_m3);

	if (!real_fields_eq) return false;

	/* lastly, we need to check the arrays :( */
	for (uint32_t i = 0; i < n_chunks; i++) {
		if (!(are_reals_equal(first.light_exposure[i], second.light_exposure[i])
			  && are_reals_equal(first.temp_celsius[i], second.temp_celsius[i])
			  && are_reals_equal(first.water_use_ml[i],
								 second.water_use_ml[i])))
			return false;
	}

	for (uint16_t i = 0, l = first.n_soil_adds.val; i < l; i++) {
		if (!are_additive_t_equal(first.soil_additives[i],
								  second.soil_additives[i]))
			return false;
	}

	for (uint16_t i = 0, l = first.n_atm_adds.val; i < l; i++) {
		if (!are_additive_t_equal(first.atm_additives[i],
								  second.atm_additives[i]))
			return false;
	}

	return true;
}

static _Bool is_additive_new(uint_t *additives, uint16_t n_additives,
							 additive_t target)
{
	for (uint16_t i = 0; i < n_additives; i++) {
		if (target.code.val == additives[i].val) return false;
	}
	return true;
}

int add_series(adf_t *adf, series_t series_to_add)
{
	series_t *last;
	size_t new_size_series;
	additive_t *soil_add, *atm_add;      
	uint16_t n_soil_add, n_atm_add, soil_addtocopy_idx, atm_addtocopy_idx;
	cpy_2_bytes_fn = is_big_endian() 
					 ? &to_big_endian_2_bytes 
					 : &to_little_endian_2_bytes;

	if (series_to_add.repeated.val == 0) { return ZERO_REPEATED_SERIES; }

	if (adf->metadata.size_series.val > 0) {
		last = adf->series + (adf->metadata.size_series.val - 1);
		if (are_series_equal(*last, series_to_add, adf->header.n_chunks.val)) {
			last->repeated.val += series_to_add.repeated.val;
			adf->metadata.n_series += series_to_add.repeated.val;
			return OK;
		}
	}

	new_size_series = (adf->metadata.size_series.val + 1) * sizeof(series_t);
	adf->series = realloc(adf->series, new_size_series);

	if (!adf->series) { return RUNTIME_ERROR; }

	n_soil_add = series_to_add.n_soil_adds.val;
	n_atm_add = series_to_add.n_atm_adds.val;
	soil_add = malloc(n_soil_add * sizeof(additive_t));
	atm_add = malloc(n_atm_add * sizeof(additive_t));
	soil_addtocopy_idx = 0;
	atm_addtocopy_idx = 0;

	if (n_soil_add > 0) {
		for (uint16_t n_soil = 0, l = n_soil_add; n_soil < l; n_soil++) {
			if (is_additive_new(adf->metadata.additive_codes,
								adf->metadata.n_additives.val,
								series_to_add.soil_additives[n_soil])) {
				soil_add[soil_addtocopy_idx]
					= series_to_add.soil_additives[n_soil];
				soil_addtocopy_idx++;
			}
		}
	}

	if (n_atm_add > 0) {
		for (uint16_t n_atm = 0, l = n_atm_add; n_atm < l; n_atm++) {
			if (is_additive_new(adf->metadata.additive_codes,
								adf->metadata.n_additives.val,
								series_to_add.atm_additives[n_atm])) {
				atm_add[atm_addtocopy_idx] = series_to_add.atm_additives[n_atm];
				atm_addtocopy_idx++;
			}
		}
	}

	uint16_t items_to_add = soil_addtocopy_idx + atm_addtocopy_idx;
	size_t new_size_additives = (adf->metadata.n_additives.val + items_to_add) 
								* sizeof(uint_t);
	adf->metadata.additive_codes = realloc(adf->metadata.additive_codes, 
										   new_size_additives);

	if (!adf->metadata.additive_codes) { return RUNTIME_ERROR; }

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
	adf->series[adf->metadata.size_series.val] = series_to_add;
	adf->metadata.size_series.val++;
	adf->metadata.n_series += series_to_add.repeated.val;

	free(soil_add);
	free(atm_add);

	return OK;
}

int remove_series(adf_t *adf)
{
	series_t *last;

	if (adf->metadata.size_series.val == 0) {
		/* nothing to do here, just return an error code */
		return EMPTY_SERIES;
	}

	last = adf->series + (adf->metadata.size_series.val - 1);

	if (last->repeated.val > 1) {
		adf->metadata.n_series--;
		last->repeated.val--;
		return OK;
	}

	adf->metadata.n_series--;
	adf->metadata.size_series.val--;
	uint32_t new_size = adf->metadata.size_series.val;

	if (new_size == 0) {
		free(adf->series);
		adf->series = NULL;
		return OK;
	}

	adf->series = realloc(adf->series, new_size * sizeof(series_t));
	if (!adf->series) { return RUNTIME_ERROR; }

	return OK;
}

int update_series(adf_t *adf, series_t series, uint64_t time)
{
	uint16_t series_period = adf->metadata.period_sec.val;
	uint64_t max_time = series_period * adf->metadata.n_series,
			 lower_bound_nth_series = 0, upper_bound_nth_series = 0;

	if (time > max_time) { return TIME_OUT_OF_BOUND; }

	for (uint32_t i = 0, l = adf->metadata.size_series.val; i < l; i++) {
		lower_bound_nth_series = upper_bound_nth_series;
		upper_bound_nth_series = (adf->series[i].repeated.val * series_period);
		if (time >= lower_bound_nth_series && time < upper_bound_nth_series)
			*(adf->series + 1) = series;
	}
	return OK;
}

static uint_t *get_additive_codes(pair_t *pairs, size_t size)
{
	uint_t *additives = malloc(size * sizeof(uint_t));
	for (size_t i = 0; i < size; i++) {
		additives[i].val = pairs[i].key;
		pairs[i].value = &i;
	}
	return additives;
}

/*
 * Since the additive code is a unique integer id we do not need an hash 
 * function. Hence, the `id` function just return the 4-byte additive code
 */
uint32_t id(void *key)
{
	return *((uint32_t *) key);
}

int reindex_additives(adf_t *adf)
{
	table_t lookup_table;
	uint16_t table_code, add_idx;
	uint32_t default_val = 1;
	pair_t *additives_keys;

	if (adf->metadata.n_series == 0) {
		adf->metadata.additive_codes = NULL;
		adf->metadata.n_additives.val = 0;
		return OK;
	}

	if ((table_code = table_init(&lookup_table, 1024, 1024, &id)) != OK)
		return REINDEX_ERROR;

	for (uint32_t i = 0, l = adf->metadata.size_series.val; i < l; i++) {
		uint16_t n_soil = adf->series[i].n_soil_adds.val;
		uint16_t n_atm = adf->series[i].n_atm_adds.val;
		for (uint16_t j = 0; j < n_soil; j++) {
			table_code = table_put(&lookup_table, 
								   adf->series[i].soil_additives[j].code.val,
								   &default_val);
			if (table_code != OK) { return REINDEX_ERROR; }
		}
	
		for (uint16_t j = 0; j < n_atm; j++) {
			table_code = table_put(&lookup_table,
								   adf->series[i].atm_additives[j].code.val,
								   &default_val);
			if (table_code != OK) { return REINDEX_ERROR; }
		}
	}

	additives_keys = table_keys(&lookup_table);
	if (!additives_keys) { return REINDEX_ERROR; }

	adf->metadata.additive_codes = get_additive_codes(additives_keys, 
													  lookup_table.size);
	adf->metadata.n_additives.val = (uint16_t)lookup_table.size;
	
	for (uint32_t i = 0, l = adf->metadata.size_series.val; i < l; i++) {
		uint16_t n_soil = adf->series[i].n_soil_adds.val;
		uint16_t n_atm = adf->series[i].n_atm_adds.val;
		additive_t *current;
		for (uint16_t j = 0; j < n_soil; j++) {
			current = (adf->series[i].soil_additives + j);
			add_idx = *((uint16_t *)table_get(&lookup_table, 
											  current->code.val));
			current->code_idx.val = add_idx;
		}
		for (uint16_t j = 0; j < n_atm; j++)
			current = (adf->series[i].atm_additives + j);
			add_idx = *((uint16_t *)table_get(&lookup_table, 
											  current->code.val));
			current->code_idx.val = add_idx;
	}

	return OK;
}

adf_header_t create_header(uint8_t farming_tec, uint32_t n_chunks,
						   uint32_t min_w_len_nm, uint32_t max_w_len_nm,
						   uint32_t n_wavelrngth)
{
	return (adf_header_t) {
	    .signature = { __ADF_SIGNATURE__ },
	    .version = __ADF_VERSION__,
	    .farming_tec = farming_tec,
	    .max_w_len_nm = { max_w_len_nm },
	    .min_w_len_nm = { min_w_len_nm },
	    .n_wavelength = { n_wavelrngth },
	    .n_chunks = { n_chunks }
	};
}

adf_meta_t create_metadata(uint32_t *additive_codes, uint16_t n_additives,
						   uint32_t size_series, uint32_t n_series,
						   uint16_t period_sec)
{
	return (adf_meta_t) {
		.additive_codes = additive_codes,
		.n_additives = n_additives,
		.size_series = size_series,
		.period_sec = period_sec,
		.n_series = n_series
	};
}

adf_t create_adf(adf_header_t header, adf_meta_t metadata)
{
	return (adf_t) { .header = header, .metadata = metadata, .series = NULL };
}

adf_t create_empty_adf(adf_header_t header)
{
	return (adf_t) { .header = header };
}
