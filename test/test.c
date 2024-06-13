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

#include "test.h"

void assert_true(bool condition, const char *label)
{
	if (condition) {
		printf(GREEN "PASSED (\u2713): %s\n" RESET, label);
	}
	else {
		printf(RED "FAILED (\u2A09): %s\n" RESET, label);
		exit(1);
	}
}

bool are_ints_equal(uint_t x, uint_t y)
{
	return x.val == y.val;
}

bool are_small_ints_equal(uint_small_t x, uint_small_t y)
{
	return x.val == y.val;
}

bool are_reals_equal(real_t x, real_t y)
{
	return x.val < y.val
			   ? (y.val - x.val) < EPSILON
			   : (x.val - y.val) < EPSILON;
}

bool are_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		if (x[i] != y[i])
			return false;

	return true;
}

bool are_int_arrays_equal(uint_t *x, uint_t *y, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		if (!are_ints_equal(x[i], y[i]))
			return false;

	return true;
}

bool are_real_arrays_equal(real_t *x, real_t *y, size_t size)
{
	for (size_t i = 0; i < size; i++)
		if (!are_reals_equal(x[i], y[i]))
			return false;

	return true;
}

bool are_additive_arrays_equal(additive_t *x, additive_t *y, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		if (!are_small_ints_equal(x[i].code, y[i].code) ||
			!are_reals_equal(x[i].concentration, y[i].concentration))
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

void assert_int_arrays_equal(uint_t *x, uint_t *y, uint32_t size, const char *label)
{
	assert_true(are_int_arrays_equal(x, y, size), label);
}

void assert_real_arrays_equal(real_t *x, real_t *y, uint32_t size, const char *label)
{
	assert_true(are_real_arrays_equal(x, y, size), label);
}

void assert_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size, const char *label)
{
	assert_true(are_uint8_arrays_equal(x, y, size), label);
}

void assert_additive_arrays_equal(additive_t *x, additive_t *y, uint32_t size, const char *label)
{
	assert_true(are_additive_arrays_equal(x, y, size), label);
}

void assert_series_equal(adf_t data, series_t x, series_t y)
{
	assert_real_arrays_equal(x.light_exposure, y.light_exposure, data.header.n_chunks.val, "are light_exposures equal");
	assert_real_arrays_equal(x.temp_celsius, y.temp_celsius, data.header.n_chunks.val, "are temp_celsiuss equal");
	assert_real_arrays_equal(x.water_use_ml, y.water_use_ml, data.header.n_chunks.val, "are water_use_mls equal");
	assert_true(x.pH == y.pH, "are pHs equal");
	assert_real_equal(x.p_bar, y.p_bar, "are pressure_pas equal");
	assert_real_equal(x.soil_density_kg_m3, y.soil_density_kg_m3, "are soil_density_t_m3s equal");
	assert_small_int_equal(x.n_soil_adds, y.n_soil_adds, "are numbers of soil additives equal");
	assert_small_int_equal(x.n_atm_adds, y.n_atm_adds, "are numbers of atmosphere additives equal");
	assert_additive_arrays_equal(x.soil_additives, y.soil_additives, x.n_soil_adds.val, "are soil additives equal");
	assert_additive_arrays_equal(x.atm_additives, y.atm_additives, x.n_atm_adds.val, "are atmosphere additives equal");
	assert_int_equal(x.repeated, y.repeated, "are repeated equal");
}

static real_t *get_real_array()
{
	real_t *light_mask = malloc(10 * sizeof(real_t));
	float f = 0.0;
	for (int i = 0; i < 10; i++, f += 0.25)
		light_mask[i].val = f;
	return light_mask;
}

adf_t get_default_object(void)
{
	additive_t *add_code = malloc(sizeof(additive_t));
	additive_t add_1 = {.code = {2345}, .concentration = {1.234}};
	*add_code = add_1;
	series_t iter1 = {
		.light_exposure = get_real_array(),
		.temp_celsius = get_real_array(),
		.water_use_ml = get_real_array(),
		.pH = 7,
		.p_bar = {0},
		.soil_density_kg_m3 = {0.345},
		.n_soil_adds = {1},
		.n_atm_adds = {0},
		.soil_additives = add_code,
		.atm_additives = NULL,
		.repeated = {1}
	};
	series_t iter2 = {
		.light_exposure = get_real_array(),
		.temp_celsius = get_real_array(),
		.water_use_ml = get_real_array(),
		.pH = 7,
		.p_bar = {0.4567},
		.soil_density_kg_m3 = {678.345},
		.n_soil_adds = {1},
		.n_atm_adds = {0},
		.soil_additives = add_code,
		.atm_additives = NULL,
		.repeated = {3}
	};
	series_t *series = malloc(2 * sizeof(series_t));
	*series = iter1;
	*(series + 1) = iter2;
	uint_t codes[1] = {{2345}};
	adf_meta_t metadata = {
		.period_sec = {1345},
		.n_additives = {1},
		.n_series = {2},
		.additive_codes = codes
	};
	adf_header_t header = {
		.signature = {__ADF_SIGNATURE__},
		.version = __ADF_VERSION__,
		.farming_tec = 3,
		.min_w_len_nm = {0},
		.max_w_len_nm = {10000},
		.n_chunks = {10},
		.n_wavelength = {10}
	};
	adf_t format = {
		.header = header,
		.metadata = metadata,
		.series = series
	};
	return format;
}
