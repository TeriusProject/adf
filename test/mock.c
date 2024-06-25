/* mock.c
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

#include "mock.h"
#include "../src/adf.h"

static real_t *get_real_array(void)
{
	real_t *light_mask = malloc(10 * sizeof(real_t));
	float f = 0.0;
	for (int i = 0; i < 10; i++, f += 0.25)
		light_mask[i].val = f;
	return light_mask;
}

series_t get_series(void)
{
	additive_t *add_code = malloc(sizeof(additive_t));
	additive_t add_1 = { .code_idx = { 2395 }, .concentration = { 1.234 } };
	*add_code = add_1;
	return (series_t){ .light_exposure = get_real_array(),
					   .temp_celsius = get_real_array(),
					   .water_use_ml = get_real_array(),
					   .pH = 11,
					   .p_bar = { 13.56789 },
					   .soil_density_kg_m3 = { 123.345 },
					   .n_soil_adds = { 1 },
					   .n_atm_adds = { 0 },
					   .soil_additives = add_code,
					   .atm_additives = NULL,
					   .repeated = { 1 } };
}

series_t get_repeated_series(void)
{
	additive_t *add_code = malloc(sizeof(additive_t));
	additive_t add_1 = { .code_idx = { 2345 }, .concentration = { 1.234 } };
	*add_code = add_1;
	return (series_t){ .light_exposure = get_real_array(),
					   .temp_celsius = get_real_array(),
					   .water_use_ml = get_real_array(),
					   .pH = 7,
					   .p_bar = { 0.4567 },
					   .soil_density_kg_m3 = { 678.345 },
					   .n_soil_adds = { 1 },
					   .n_atm_adds = { 0 },
					   .soil_additives = add_code,
					   .atm_additives = NULL,
					   .repeated = { 2 } };
}

adf_t get_object_with_zero_series(void)
{
	uint_t *codes = malloc(sizeof(uint_t));
	*codes = (uint_t){ 2345 };
	adf_meta_t metadata = { .period_sec = { 1345 },
							.n_additives = { 1 },
							.size_series = { 0 },
							.n_series = 0,
							.additive_codes = codes };
	adf_header_t header = { .signature = { __ADF_SIGNATURE__ },
							.version = __ADF_VERSION__,
							.farming_tec = 3,
							.min_w_len_nm = { 0 },
							.max_w_len_nm = { 10000 },
							.n_chunks = { 10 },
							.n_wavelength = { 10 } };
	adf_t format = { .header = header, .metadata = metadata, .series = NULL };
	return format;
}

series_t *get_default_series(void)
{
	additive_t *add_code = malloc(sizeof(additive_t));
	additive_t add_1 = { .code_idx = { 2345 }, .concentration = { 1.234 } };
	*add_code = add_1;
	series_t iter1 = { .light_exposure = get_real_array(),
					   .temp_celsius = get_real_array(),
					   .water_use_ml = get_real_array(),
					   .pH = 7,
					   .p_bar = { 0 },
					   .soil_density_kg_m3 = { 0.345 },
					   .n_soil_adds = { 1 },
					   .n_atm_adds = { 0 },
					   .soil_additives = add_code,
					   .atm_additives = NULL,
					   .repeated = { 1 } };
	series_t iter2 = { .light_exposure = get_real_array(),
					   .temp_celsius = get_real_array(),
					   .water_use_ml = get_real_array(),
					   .pH = 7,
					   .p_bar = { 0.4567 },
					   .soil_density_kg_m3 = { 678.345 },
					   .n_soil_adds = { 1 },
					   .n_atm_adds = { 0 },
					   .soil_additives = add_code,
					   .atm_additives = NULL,
					   .repeated = { 3 } };
	series_t *series = malloc(2 * sizeof(series_t));
	*series = iter1;
	*(series + 1) = iter2;
	return series;
}

adf_t get_default_object(void)
{
	uint_t *codes = malloc(sizeof(uint_t));
	*codes = (uint_t){ 2345 };
	adf_meta_t metadata = { .period_sec = { 1345 },
							.n_additives = { 1 },
							.size_series = { 2 },
							.n_series = 4,
							.additive_codes = codes };
	adf_header_t header = { .signature = { __ADF_SIGNATURE__ },
							.version = __ADF_VERSION__,
							.farming_tec = 3,
							.min_w_len_nm = { 0 },
							.max_w_len_nm = { 10000 },
							.n_chunks = { 10 },
							.n_wavelength = { 10 } };
	adf_t format = { .header = header,
					 .metadata = metadata,
					 .series = get_default_series() };
	return format;
}