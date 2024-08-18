/* test_comparisons.c
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

#include "../src/adf.h"
#include "test.h"
#include "mock.h"

void compare_series_with_zero_tolerance(void)
{
	adf_t adf = get_default_object();
	series_t *series = get_default_series();

	assert_true(are_series_equal(adf.series, series, &adf),
				"\u0394 = 0: the two series should be equal");

	series->soil_density_kg_m3.val += 1e2;
	assert_true(!are_series_equal(adf.series, series, &adf),
				"\u0394 = 0: increment of 1e2 -> series not equal");
}

void compare_series_with_tolerance(void)
{
	adf_t adf = get_object_with_precision_set();
	series_t *series = get_default_series();
	uint16_t n_wavelength = adf.header.wave_info.n_wavelength.val,
		n_depth = adf.header.soil_info.n_depth.val;
	uint32_t n_chunks = adf.header.n_chunks.val;

	series->soil_density_kg_m3.val += 0.7;
	series->p_bar.val += 0.05;
	for (uint32_t i = 0; i < adf.header.n_chunks.val; i++) {
		series->water_use_ml[i].val += 0.9;
		series->env_temp_c[i].val += 0.4002;
	}

	for (uint64_t i = 0; i < n_wavelength * n_chunks; i++) {
		series->light_exposure[i].val += 0.3;
	}

	for (uint64_t i = 0; i < n_depth * n_chunks; i++) {
		series->soil_temp_c[i].val += 4.7954;
	}

	for (uint16_t i = 0; i < series->n_soil_adds.val; i++) {
		series->soil_additives[i].concentration.val += 0.999;
	}

	for (uint16_t i = 0; i < series->n_atm_adds.val; i++) {
		series->atm_additives[i].concentration.val += 0.999;
	}

	assert_true(are_series_equal(adf.series, series, &adf),
				"\u0394 \u2264 tol: the two series should be equal");

	series->soil_density_kg_m3.val += 1.001;

	assert_true(!are_series_equal(adf.series, series, &adf),
				"\u0394 \u2265 tol: the two series should *not* be equal");
}

int main(void)
{
	compare_series_with_zero_tolerance();
	compare_series_with_tolerance();
}
