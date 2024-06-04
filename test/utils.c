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

#include "utils.h"

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

void assert_real_arrays_equal(real_t *x, real_t *y, uint32_t size, const char *label)
{
	assert_true(are_real_arrays_equal(x, y, size), label);
}

void assert_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size, const char *label)
{
	assert_true(are_uint8_arrays_equal(x, y, size), label);
}

void assert_iter_equal(uint32_t n, adf_t data, iter_t x, iter_t y)
{
	printf("(iteration %u)\n", n);
	assert_real_arrays_equal(x.light_exposure, y.light_exposure, data.n_chunks.val, "are light_exposures equal");
	assert_real_arrays_equal(x.temp_celsius, y.temp_celsius, data.n_chunks.val, "are temp_celsiuss equal");
	assert_real_arrays_equal(x.water_use_ml, y.water_use_ml, data.n_chunks.val, "are water_use_mls equal");
	assert_uint8_arrays_equal(x.light_wavelength, y.light_wavelength, data.n_wavelength.val, "are light_wavelengths equal");
	assert_real_equal(x.pH, y.pH, "are pHs equal");
	assert_real_equal(x.pressure_pa, y.pressure_pa, "are pressure_pas equal");
	assert_real_equal(x.soil_density_t_m3, y.soil_density_t_m3, "are soil_density_t_m3s equal");
	assert_real_equal(x.nitrogen_g_m3, y.nitrogen_g_m3, "are nitrogen_g_m3s equal");
	assert_real_equal(x.potassium_g_m3, y.potassium_g_m3, "are potassium_g_m3s equal");
	assert_real_equal(x.phosphorus_g_m3, y.phosphorus_g_m3, "are phosphorus_g_m3s equal");
	assert_real_equal(x.iron_g_m3, y.iron_g_m3, "are iron_g_m3s equal");
	assert_real_equal(x.magnesium_g_m3, y.magnesium_g_m3, "are magnesium_g_m3s equal");
	assert_real_equal(x.sulfur_g_m3, y.sulfur_g_m3, "are sulfur_g_m3s equal");
	assert_real_equal(x.calcium_g_m3, y.calcium_g_m3, "are calcium_g_m3s equal");
}

static real_t *get_real_array()
{
	real_t *light_mask = malloc(10 * sizeof(real_t));
	float f = 0.0;
	for (int i = 0; i < 10; i++, f += 0.25)
		light_mask[i].val = f;
	return light_mask;
}

static uint8_t *get_int_array()
{
	uint8_t *light_wavelengths = malloc(10 * sizeof(uint8_t));
	for (uint8_t i = 0; i < 10; i++)
		light_wavelengths[i] = i;
	return light_wavelengths;
}

adf_t get_default_object(void)
{
	iter_t iter1 = {
		.light_exposure = get_real_array(),
		.temp_celsius = get_real_array(),
		.water_use_ml = get_real_array(),
		.light_wavelength = get_int_array(),
		.pH = {11.0},
		.pressure_pa = {0},
		.soil_density_t_m3 = {0},
		.nitrogen_g_m3 = {0},
		.potassium_g_m3 = {3.8},
		.phosphorus_g_m3 = {0},
		.iron_g_m3 = {0},
		.magnesium_g_m3 = {0},
		.sulfur_g_m3 = {5.5},
		.calcium_g_m3 = {0},
	};
	iter_t iter2 = {
		.light_exposure = get_real_array(),
		.temp_celsius = get_real_array(),
		.water_use_ml = get_real_array(),
		.light_wavelength = get_int_array(),
		.pH = {8.0},
		.pressure_pa = {0},
		.soil_density_t_m3 = {0},
		.nitrogen_g_m3 = {0},
		.potassium_g_m3 = {3.8},
		.phosphorus_g_m3 = {0},
		.iron_g_m3 = {1},
		.magnesium_g_m3 = {0},
		.sulfur_g_m3 = {5.5},
		.calcium_g_m3 = {6.23567},
	};
	iter_t *iterations = malloc(2 * sizeof(iter_t));
	*iterations = iter1;
	*(iterations + 1) = iter2;
	adf_t format = {
		.signature = {__ADF_SIGNATURE__},
		.version = __ADF_VERSION__,
		.min_w_len_nm = {0},
		.max_w_len_nm = {10000},
		.period_sec = {1245637},
		.n_chunks = {10},
		.n_wavelength = {10},
		.n_iterations = {2},
		.iterations = iterations
	};
	return format;
}
