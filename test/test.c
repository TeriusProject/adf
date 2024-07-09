/* test.c - A minimal unit test framework implementation
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

#include "test.h"
#include <time.h>

void assert_true(bool condition, const char *label)
{
	if (condition) { printf(GREEN "PASSED (" TICK "): %s" RESET "\n", label); }
	else {
		printf(RED "FAILED (" CROSS "): %s\n" RESET, label);
		exit(1);
	}
}

bool are_ints_equal(uint_t x, uint_t y) { return x.val == y.val; }

bool are_small_ints_equal(uint_small_t x, uint_small_t y)
{
	return x.val == y.val;
}

bool are_reals_equal(real_t x, real_t y)
{
	return x.val < y.val ? (y.val - x.val) < EPSILON
						 : (x.val - y.val) < EPSILON;
}

bool are_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		if (x[i] != y[i]) return false;

	return true;
}

bool are_uint32_arrays_equal(uint32_t *x, uint32_t *y, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		if (x[i] != y[i]) return false;

	return true;
}

bool are_pair_arrays_equal(pair_t *x, pair_t *y, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		if (x[i].key != y[i].key) return false;

	return true;
}

bool are_int_arrays_equal(uint_t *x, uint_t *y, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		if (!are_ints_equal(x[i], y[i])) return false;

	return true;
}

bool are_real_arrays_equal(real_t *x, real_t *y, size_t size)
{
	for (size_t i = 0; i < size; i++)
		if (!are_reals_equal(x[i], y[i])) return false;

	return true;
}

bool are_additives_equal(additive_t x, additive_t y)
{
	return are_small_ints_equal(x.code_idx, y.code_idx)
		   && are_reals_equal(x.concentration, y.concentration);
}

bool are_additive_arrays_equal(additive_t *x, additive_t *y, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		if (!are_additives_equal(x[i], y[i]))
			return false;
	}
	return true;
}

void assert_int_equal(uint_t x, uint_t y, const char *label)
{
	assert_true(x.val == y.val, label);
}

void assert_long_equal(long x, long y, const char *label)
{
	assert_true(x == y, label);
}

void assert_real_equal(real_t x, real_t y, const char *label)
{
	assert_true(are_reals_equal(x, y), label);
}

void assert_small_int_equal(uint_small_t x, uint_small_t y, const char *label)
{
	assert_true(are_small_ints_equal(x, y), label);
}

void assert_int_arrays_equal(uint_t *x, uint_t *y, uint32_t size, 
							 const char *label)
{
	assert_true(are_int_arrays_equal(x, y, size), label);
}

void assert_uint32_arrays_equal(uint32_t *x, uint32_t *y, uint32_t size, 
								const char *label)
{
	assert_true(are_uint32_arrays_equal(x, y, size), label);
}

void assert_pair_arrays_equal(pair_t *x, pair_t *y, uint32_t size, 
							  const char *label)
{
	assert_true(are_pair_arrays_equal(x, y, size), label);
}

void assert_real_arrays_equal(real_t *x, real_t *y, uint32_t size,
							  const char *label)
{
	assert_true(are_real_arrays_equal(x, y, size), label);
}

void assert_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size,
							   const char *label)
{
	assert_true(are_uint8_arrays_equal(x, y, size), label);
}

void assert_additive_arrays_equal(additive_t *x, additive_t *y, uint32_t size,
								  const char *label)
{
	assert_true(are_additive_arrays_equal(x, y, size), label);
}

void assert_header_equal_verbose(adf_header_t target, adf_header_t expected)
{
	assert_int_equal(
		target.signature, expected.signature, "are signatures equals"
	);
	assert_true(target.version == expected.version, "are versions equals");
	assert_true(
		target.farming_tec == expected.farming_tec,
		"are farming tecniques equals"
	);
	assert_int_equal(
		target.n_wavelength, expected.n_wavelength, "are n_wavelengths equal"
	);
	assert_int_equal(
		target.min_w_len_nm, expected.min_w_len_nm, "are min_w_len_nms equal"
	);
	assert_int_equal(
		target.max_w_len_nm, expected.max_w_len_nm, "are max_w_len_nms equal"
	);
	assert_int_equal(target.n_chunks, expected.n_chunks, "are n_chunks equal");
}

void assert_header_equal(adf_header_t target, adf_header_t expected,
						 const char *label)
{
	bool c = are_ints_equal(target.signature, expected.signature)
			 && target.version == expected.version
			 && target.farming_tec == expected.farming_tec
			 && are_ints_equal(target.n_wavelength, expected.n_wavelength)
			 && are_ints_equal(target.min_w_len_nm, expected.min_w_len_nm)
			 && are_ints_equal(target.max_w_len_nm, expected.max_w_len_nm)
			 && are_ints_equal(target.n_chunks, expected.n_chunks);
	assert_true(c, label);
}

void assert_metadata_equal(adf_meta_t target, adf_meta_t expected,
						   const char *label)
{
	bool c = are_ints_equal(target.size_series, expected.size_series)
			 && target.n_series == expected.n_series
			 && are_ints_equal(target.period_sec, expected.period_sec)
			 && are_small_ints_equal(target.n_additives, expected.n_additives)
			 && are_int_arrays_equal(target.additive_codes, 
			 						 expected.additive_codes,
									 target.n_additives.val);
	assert_true(c, label);
}

void assert_metadata_equal_verbose(adf_meta_t target, adf_meta_t expected)
{
	assert_int_equal(
		target.size_series, expected.size_series, "are size_series equal"
	);
	assert_true(target.n_series == expected.n_series, "are n_series equal");
	assert_int_equal(target.period_sec, expected.period_sec, 
					 "are periods equal");
	assert_small_int_equal(target.n_additives, expected.n_additives,
						   "are number of additive codes equal");
	assert_int_arrays_equal(target.additive_codes, expected.additive_codes,
							target.n_additives.val,
							"are all the additive codes equal");
}

void assert_series_equal_verbose(adf_t data, series_t x, series_t y)
{
	assert_real_arrays_equal(
		x.light_exposure, y.light_exposure, data.header.n_chunks.val,
		"are light_exposures equal"
	);
	assert_real_arrays_equal(
		x.temp_celsius, y.temp_celsius, data.header.n_chunks.val,
		"are temp_celsiuss equal"
	);
	assert_real_arrays_equal(
		x.water_use_ml, y.water_use_ml, data.header.n_chunks.val,
		"are water_use_mls equal"
	);
	assert_true(x.pH == y.pH, "are pHs equal");
	assert_real_equal(x.p_bar, y.p_bar, "are pressure_pas equal");
	assert_real_equal(
		x.soil_density_kg_m3, y.soil_density_kg_m3,
		"are soil_density_t_m3s equal"
	);
	assert_small_int_equal(
		x.n_soil_adds, y.n_soil_adds, "are numbers of soil additives equal"
	);
	assert_small_int_equal(
		x.n_atm_adds, y.n_atm_adds, "are numbers of atmosphere additives equal"
	);
	assert_additive_arrays_equal(
		x.soil_additives, y.soil_additives, x.n_soil_adds.val,
		"are soil additives equal"
	);
	assert_additive_arrays_equal(
		x.atm_additives, y.atm_additives, x.n_atm_adds.val,
		"are atmosphere additives equal"
	);
	assert_int_equal(x.repeated, y.repeated, "are repeated equal");
}

void assert_series_equal(adf_t data, series_t x, series_t y, const char *label)
{
	bool c = are_real_arrays_equal(x.light_exposure, y.light_exposure,
								   data.header.n_chunks.val)
			&& are_real_arrays_equal(x.temp_celsius, y.temp_celsius,
									 data.header.n_chunks.val)
			&& are_real_arrays_equal(x.water_use_ml, y.water_use_ml,
 									 data.header.n_chunks.val)
			&& x.pH == y.pH
			&& are_reals_equal(x.p_bar, y.p_bar)
			&& are_reals_equal(x.soil_density_kg_m3, y.soil_density_kg_m3)
			&& are_small_ints_equal(x.n_soil_adds, y.n_soil_adds)
			&& are_small_ints_equal(x.n_atm_adds, y.n_atm_adds)
			&& are_additive_arrays_equal(x.soil_additives, y.soil_additives,
			  							 x.n_soil_adds.val)
			&& are_additive_arrays_equal(x.atm_additives, y.atm_additives,
			  							 x.n_atm_adds.val)
			&& are_ints_equal(x.repeated, y.repeated);
	assert_true(c, label);
}

uint64_t get_nanos(void)
{
	tick_t start;
	clock_gettime(CLOCK_MONOTONIC_RAW, &start);
	return (uint64_t) start.tv_sec*1e9 + (uint64_t) start.tv_nsec;
}

double get_time_diff(uint64_t prev_tick)
{
	return (get_nanos() - prev_tick)*1e-9;
}
