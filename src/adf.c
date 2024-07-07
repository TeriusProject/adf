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
#include <stdio.h>
#include <stdlib.h>

#define SHIFT_COUNTER(n) (byte_c += n)

typedef void (*number_bytes_copy)(uint8_t *, const uint8_t *);

/*
 * Hash function.
 * Since the additive code is a unique integer id we do not need an hash 
 * function. Hence, the `id` function just return the 4-byte additive code
 */
static uint32_t id(void *key)
{
	return *((uint32_t *) key);
}

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
		   + 4								/* period_sec */
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

uint8_t *adf_bytes_alloc(adf_t data) { return (uint8_t *)malloc(size_adf_t(data)); }

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
	cpy_4_bytes_fn((bytes + byte_c), data.metadata.period_sec.bytes);
	SHIFT_COUNTER(4);
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
		if (!current.light_exposure) { return ADF_RUNTIME_ERROR; }
		for (uint32_t mask_i = 0; mask_i < data.header.n_wavelength.val;
			 mask_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c),
						   current.light_exposure[mask_i].bytes);
		}
		if (!current.temp_celsius) { return ADF_RUNTIME_ERROR; }
		for (uint32_t temp_i = 0; temp_i < data.header.n_chunks.val;
			 temp_i++, byte_c += 4) {
			cpy_4_bytes_fn((bytes + byte_c),
						   current.temp_celsius[temp_i].bytes);
		}
		if (!current.water_use_ml) { return ADF_RUNTIME_ERROR; }
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
	return ADF_OK;
}

uint16_t unmarshal(adf_t *adf, const uint8_t *bytes)
{
	size_t byte_c = 0;
	uint_small_t expected_crc;
	uint16_t header_crc, meta_crc, series_crc;
	uint32_t n_series = 0, n_iter, n_chunks, n_waves;
	cpy_4_bytes_fn = is_big_endian()
					 ? &to_big_endian_4_bytes 
					 : &to_little_endian_4_bytes;
	cpy_2_bytes_fn = is_big_endian()
					 ? &to_big_endian_2_bytes 
					 : &to_little_endian_2_bytes;

	if (!bytes || !adf) { return ADF_RUNTIME_ERROR; }

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
	header_crc = crc16(bytes, byte_c);
	cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);

	if (header_crc != expected_crc.val) { return ADF_HEADER_CORRUPTED; }

	cpy_4_bytes_fn(adf->metadata.size_series.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	n_series = adf->metadata.size_series.val;
	cpy_4_bytes_fn(adf->metadata.period_sec.bytes, (bytes + byte_c));
	SHIFT_COUNTER(4);
	cpy_2_bytes_fn(adf->metadata.n_additives.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);

	adf->metadata.additive_codes = malloc(adf->metadata.n_additives.val 
										  * sizeof(uint_t));
	if (!adf->metadata.additive_codes) { return ADF_RUNTIME_ERROR; }

	for (uint16_t i = 0, l = adf->metadata.n_additives.val; i < l;
		 i++, byte_c += 4) {
		cpy_4_bytes_fn(adf->metadata.additive_codes[i].bytes, (bytes + byte_c));
	}

	meta_crc = crc16((bytes + size_header()), byte_c - size_header());
	cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
	SHIFT_COUNTER(2);
	if (meta_crc != expected_crc.val) { return ADF_METADATA_CORRUPTED; }

	adf->series = malloc(adf->metadata.size_series.val * sizeof(series_t));
	if (!adf->series) { return ADF_RUNTIME_ERROR; }

	n_iter = adf->metadata.size_series.val;
	n_chunks = adf->header.n_chunks.val;
	n_waves = adf->header.n_wavelength.val;

	for (uint32_t i = 0; i < n_iter; i++) {
		series_t current;
		size_t starting_byte = byte_c;
		current.light_exposure = malloc(n_waves * sizeof(real_t));
		if (!current.light_exposure) { return ADF_RUNTIME_ERROR; }

		current.temp_celsius = malloc(n_chunks * sizeof(real_t));
		if (!current.temp_celsius) { return ADF_RUNTIME_ERROR; }

		current.water_use_ml = malloc(n_chunks * sizeof(real_t));
		if (!current.water_use_ml) { return ADF_RUNTIME_ERROR; }

		for (u_int32_t mask_i = 0; mask_i < adf->header.n_wavelength.val;
			 mask_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.light_exposure[mask_i].bytes,
						   (bytes + byte_c));
		}
		for (u_int32_t temp_i = 0; temp_i < n_chunks; temp_i++, byte_c += 4) {
			cpy_4_bytes_fn(current.temp_celsius[temp_i].bytes,
						   (bytes + byte_c));
		}
		for (u_int32_t w_i = 0; w_i < n_chunks; w_i++, byte_c += 4) {
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

		if (current.repeated.val == 0) { return ADF_ZERO_REPEATED_SERIES; }
		
		series_crc = crc16((bytes + starting_byte), byte_c - starting_byte);
		cpy_2_bytes_fn(expected_crc.bytes, (bytes + byte_c));
		SHIFT_COUNTER(2);

		if (series_crc != expected_crc.val) { return ADF_SERIES_CORRUPTED; }
		n_series += current.repeated.val - 1;
		adf->series[i] = current;
	}
	adf->metadata.n_series = n_series;
	return ADF_OK;
}

static inline bool are_reals_equal(real_t x, real_t y)
{
	return x.val < y.val ? (y.val - x.val) < EPSILON
						 : (x.val - y.val) < EPSILON;
}

static bool are_additive_t_equal(additive_t x, additive_t y)
{
	return x.code.val == y.code.val && x.code_idx.val == y.code_idx.val
		   && are_reals_equal(x.concentration, y.concentration);
}

bool are_series_equal(const series_t *first, const series_t *second, 
					  const adf_t *adf)
{
	uint32_t n_chunks = adf->header.n_chunks.val;
	uint16_t n_wavelength = adf->header.n_wavelength.val;
	bool int_fields_eq = first->pH == second->pH
						 && first->n_atm_adds.val == second->n_atm_adds.val
						 && first->n_soil_adds.val == second->n_soil_adds.val;

	if (!int_fields_eq) return false;

	bool real_fields_eq = are_reals_equal(first->p_bar, second->p_bar)
						  && are_reals_equal(first->soil_density_kg_m3,
											 second->soil_density_kg_m3);

	if (!real_fields_eq) return false;

	/* lastly, we need to check the arrays :( */
	for (uint32_t i = 0; i < n_chunks; i++) {
		if (!(are_reals_equal(first->temp_celsius[i], second->temp_celsius[i])
			  && are_reals_equal(first->water_use_ml[i],
								 second->water_use_ml[i])))
			return false;
	}

	for (uint16_t i = 0; i < n_wavelength; i++) {
		if (!are_reals_equal(first->light_exposure[i], second->light_exposure[i]))
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

	/* Happy path, the series is repeated, just increment */
	if (adf->metadata.size_series.val > 0) {
		last = adf->series + (adf->metadata.size_series.val - 1);
		if (are_series_equal(last, series_to_add, adf)) {
			last->repeated.val += series_to_add->repeated.val;
			adf->metadata.n_series += series_to_add->repeated.val;
			return ADF_OK;
		}
	}

	/* If it's not equal to the last one, and if it's not zero-repeated, 
	   then we have to add it to the series array */
	new_size_series = (adf->metadata.size_series.val + 1) * sizeof(series_t);
	adf->series = realloc(adf->series, new_size_series);
	if (!adf->series) { return ADF_RUNTIME_ERROR; }

	last = adf->series + adf->metadata.size_series.val;
	res = cpy_adf_series(last, series_to_add, adf);
	if (res != ADF_OK) { return res; }

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
		series_free(last);
		if (n_soil_add > 0) { free(soil_add); }
		if (n_atm_add > 0) { free(atm_add); }
		return ADF_ADDITIVE_OVERFLOW; 
	}
	size_t new_size_additives = (adf->metadata.n_additives.val + items_to_add) 
								* sizeof(uint_t);
	adf->metadata.additive_codes = realloc(adf->metadata.additive_codes, 
										   new_size_additives);

	if (!adf->metadata.additive_codes) {
		series_free(last);
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

	series_free(last);

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

uint16_t update_series(adf_t *adf, const series_t *series, uint64_t time)
{
	uint16_t series_period = adf->metadata.period_sec.val;
	uint32_t new_series_size, size_series_increment;
	uint64_t max_time = series_period * adf->metadata.n_series,
			 l_bound_nth_series = 0, u_bound_nth_series = 0;
	series_t *current;

	if (time > max_time) { return ADF_TIME_OUT_OF_BOUND; }

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

		if (current->repeated.val == 1) {
			adf->metadata.n_series += (series->repeated.val 
									  - current->repeated.val);
			series_free(current);
			cpy_adf_series(current, series, adf);
			return ADF_OK;
		}

		for (uint32_t j = 0, len = current->repeated.val; j < len; j++) {
			if (time < l_bound_nth_series || time >= u_bound_nth_series)
				continue;
			
			size_series_increment = (j == l - 1) ? 1 : 2;
			new_series_size = adf->metadata.size_series.val
							  + size_series_increment;
			
			adf->series = realloc(adf->series, new_series_size);
			if (!adf->series) {
				return ADF_RUNTIME_ERROR;
			}

			cpy_adf_series(adf->series + (i+1), series, adf);
			adf->series[i+2].repeated.val = len - i;
			if (size_series_increment == 2) {
				cpy_adf_series(adf->series + (i+2), adf->series + i, adf);
				adf->series[i+2].repeated.val = 1;
			}

			l_bound_nth_series += series_period; 
			u_bound_nth_series += series_period;
		}
	}
	
	return ADF_OK;
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

uint16_t reindex_additives(adf_t *adf)
{
	table_t lookup_table;
	uint16_t table_code, add_idx;
	uint32_t default_val = 1;
	pair_t *additives_keys;

	if (adf->metadata.n_series == 0) {
		adf->metadata.additive_codes = NULL;
		adf->metadata.n_additives.val = 0;
		return ADF_OK;
	}

	if ((table_code = table_init(&lookup_table, 1024, 1024, &id)) != LM_OK)
		return ADF_RUNTIME_ERROR;

	for (uint32_t i = 0, l = adf->metadata.size_series.val; i < l; i++) {
		uint16_t n_soil = adf->series[i].n_soil_adds.val;
		uint16_t n_atm = adf->series[i].n_atm_adds.val;
		for (uint16_t j = 0; j < n_soil; j++) {
			table_code = table_put(&lookup_table, 
								   adf->series[i].soil_additives[j].code.val,
								   &default_val);
			if (table_code == LM_MAP_SIZE_OVERFLOW) {
				return ADF_ADDITIVE_OVERFLOW;
			} else if (table_code != LM_OK) {
				return ADF_RUNTIME_ERROR; 
			}
		}
	
		for (uint16_t j = 0; j < n_atm; j++) {
			table_code = table_put(&lookup_table,
								   adf->series[i].atm_additives[j].code.val,
								   &default_val);
			if (table_code == LM_MAP_SIZE_OVERFLOW) {
				return ADF_ADDITIVE_OVERFLOW;
			} else if (table_code != LM_OK) {
				return ADF_RUNTIME_ERROR; 
			}
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
	    .version = __ADF_VERSION__,
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
						   uint32_t n_wavelenght, uint16_t n_soil_additives,
						   uint16_t n_atm_additives)
{
	series->n_soil_adds.val = n_soil_additives;
	series->n_atm_adds.val = n_atm_additives;
	series->temp_celsius = malloc(n_chunks * sizeof(real_t));
	series->water_use_ml = malloc(n_chunks * sizeof(real_t));
	series->light_exposure = malloc(n_wavelenght * sizeof(real_t));
	series->soil_additives = malloc(n_soil_additives * sizeof(additive_t));
	series->atm_additives = malloc(n_atm_additives * sizeof(additive_t));
	
	if (!series->temp_celsius  
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

void series_free(series_t *series)
{ 
	free(series->light_exposure);
	free(series->temp_celsius);
	free(series->water_use_ml);
	if (series->n_soil_adds.val > 0) { free(series->soil_additives); }
	if (series->n_atm_adds.val > 0) { free(series->atm_additives); }

	series->light_exposure = NULL;
	series->temp_celsius = NULL;
	series->water_use_ml = NULL;
	series->soil_additives = NULL;
	series->atm_additives = NULL;
}

void adf_free(adf_t *adf)
{
	adf_meta_t *metadata = (adf_meta_t *) &(adf->metadata);
	metadata_free(metadata);
	for (uint32_t i = 0, l = adf->metadata.size_series.val; i < l; i++) 
		series_free(adf->series + i);
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
	uint16_t n_wavelength;

	if (!source) { return ADF_NULL_SERIES_SOURCE; }
	if (!target) { return ADF_NULL_SERIES_TARGET; }

	n_chunks = adf->header.n_chunks.val;
	n_wavelength = adf->header.n_wavelength.val;
	target->n_atm_adds = source->n_atm_adds;
	target->n_soil_adds = source->n_soil_adds;
	target->p_bar = source->p_bar;
	target->pH = source->pH;
	target->repeated = source->repeated;
	target->soil_density_kg_m3 = source->soil_density_kg_m3;

	target->water_use_ml = malloc(n_chunks * sizeof(real_t));
	target->temp_celsius = malloc(n_chunks * sizeof(real_t));
	for (uint32_t i = 0; i < n_chunks; i++) {
		target->water_use_ml[i] = source->water_use_ml[i];
		target->temp_celsius[i] = source->temp_celsius[i];
	}

	target->light_exposure = malloc(n_wavelength * sizeof(real_t));
	for (uint32_t i = 0; i < n_wavelength; i++) 
		target->light_exposure[i] = source->light_exposure[i];

	if (target->n_soil_adds.val > 0) {
		target->soil_additives = malloc(target->n_soil_adds.val
									* sizeof(additive_t));
		for (uint16_t i = 0, l = target->n_soil_adds.val; i < l; i++)
			target->soil_additives[i] = source->soil_additives[i];
	}
	else {
		target->soil_additives = NULL;
	}

	if (target->n_atm_adds.val > 0) {
		target->atm_additives = malloc(target->n_atm_adds.val
								   * sizeof(additive_t));
		for (uint16_t i = 0, l = target->n_atm_adds.val; i < l; i++)
			target->atm_additives[i] = source->atm_additives[i];
	}
	else {
		target->atm_additives = NULL;
	}

	return ADF_OK;
}

uint16_t cpy_adf_metadata(adf_meta_t *target, const adf_meta_t *source)
{
	if (!source) { return ADF_NULL_META_SOURCE; }
	if (!target) { return ADF_NULL_META_TARGET; }

	target->size_series = source->size_series;
	target->period_sec = source->period_sec;
	target->n_series = source->n_series;
	target->n_additives = source->n_additives;
	target->additive_codes = malloc(target->n_additives.val
									* sizeof(uint_t));

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
