/* 
 * mock.c
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

#include "mock.h"
#include "../src/adf.h"

real_t *get_real_array(int n_chunks)
{
	real_t *light_mask = malloc(n_chunks * sizeof(real_t));
	float f = 0.0;
	for (int i = 0; i < n_chunks; i++, f += 0.25)
		light_mask[i].val = f;
	return light_mask;
}

real_t *get_real_inline_matrix(int n_rows, int n_columns)
{
	float f;
	real_t *mat = malloc(n_rows * n_columns* sizeof(real_t));

	for (int i = 0; i < n_rows; i++) {
		f = 0.0;
		for (int j = 0; j < n_columns; j++, f += 0.25) {
			mat[j + i * n_columns].val = f;
		}
	}
	return mat;
}

real_t **get_real_matrix(int n_rows, int n_columns)
{
	float f;
	real_t **light_mask = malloc(n_rows * sizeof(real_t *));

	for (int i = 0; i < n_rows; i++) {
		f = 0.0;
		light_mask[i] = malloc(n_columns * sizeof(real_t));
		for (int j = 0; j < n_columns; j++, f += 0.25) {
			light_mask[i][j].val = f;
		}
	}
	return light_mask;
}

series_t get_series(void)
{
	additive_t *soil_add_code = malloc(sizeof(additive_t));
	*soil_add_code = (additive_t) {
		.code = { 1234 }, 
		.concentration = { 1.234 }
	};
	return (series_t) { 
		.light_exposure = get_real_inline_matrix(10, 20),
		.soil_temp_c = get_real_inline_matrix(10, 2),
		.env_temp_c = get_real_array(10),
		.water_use_ml = get_real_array(10),
		.pH = 11,
		.p_bar = { 13.56789 },
		.soil_density_kg_m3 = { 123.345 },
		.n_soil_adds = { 1 },
		.n_atm_adds = { 0 },
		.soil_additives = soil_add_code,
		.atm_additives = NULL,
		.repeated = { 1 }
	};
}

series_t get_repeated_series(void)
{
	additive_t *add_code;

	add_code = malloc(sizeof(additive_t));
	*add_code = (additive_t) {
		.code = { 2345 },
		.concentration = { 3.33 }
	};

	return (series_t) { 
		.light_exposure = get_real_inline_matrix(10, 20),
		.soil_temp_c = get_real_inline_matrix(10, 2),
		.env_temp_c = get_real_array(10),
		.water_use_ml = get_real_array(10),
		.pH = 7,
		.p_bar = { 0.4567 },
		.soil_density_kg_m3 = { 678.345 },
		.n_soil_adds = { 1 },
		.n_atm_adds = { 0 },
		.soil_additives = add_code,
		.atm_additives = NULL,
		.repeated = { 2 }
	};
}

series_t get_series_with_two_soil_additives(void)
{
	additive_t *add_code = malloc(2 * sizeof(additive_t));
	additive_t add_1 = { .code = { 2345 }, .concentration = { 1.234 } };
	additive_t add_2 = { .code = { 6789 }, .concentration = { 6.789 } };
	*add_code = add_1;
	*add_code = add_2;
	return (series_t) { 
		.light_exposure = get_real_inline_matrix(10, 20),
		.soil_temp_c = get_real_inline_matrix(10, 2),
		.env_temp_c = get_real_array(10),
		.water_use_ml = get_real_array(10),
		.pH = 7,
		.p_bar = { 0.4567 },
		.soil_density_kg_m3 = { 678.345 },
		.n_soil_adds = { 2 },
		.n_atm_adds = { 0 },
		.soil_additives = add_code,
		.atm_additives = NULL,
		.repeated = { 2 }
	};
}

adf_t get_object_with_zero_series(void)
{
	return (adf_t) {
		.header = (adf_header_t) { 
			.signature = { __ADF_SIGNATURE__ },
			.version = { __ADF_VERSION__ },
			.farming_tec = 3,
			.min_w_len_nm = { 0 },
			.max_w_len_nm = { 10000 },
			.n_chunks = { 10 },
			.n_wavelength = { 10 },
			.n_depth = {2},
			.min_soil_depth_mm = { 0 },
			.max_soil_depth_mm = { 20} 
		},
		.metadata = (adf_meta_t) {
			.period_sec = { 1345 },
			.n_additives = { 0 },
			.size_series = { 0 },
			.n_series = 0,
			.additive_codes = NULL
		},
		.series = NULL
	};
}

series_t *get_default_series(void)
{
	additive_t *add_code_series_1, *add_code_series_2;
	series_t *series;
	series_t iter1, iter2;

	add_code_series_1 = malloc(sizeof(additive_t));
	*add_code_series_1 = (additive_t) {
		.code = { 2345 },
		.concentration = { 1.234 }
	};
	
	add_code_series_2 = malloc(sizeof(additive_t));
	*add_code_series_2 = (additive_t) {
		.code = { 2345 },
		.concentration = { 3.33 }
	};

	iter1 = (series_t) { 
		.light_exposure = get_real_inline_matrix(10, 20),
		.soil_temp_c = get_real_inline_matrix(10, 2),
		.env_temp_c = get_real_array(10),
		.water_use_ml = get_real_array(10),
		.pH = 7,
		.p_bar = { 0 },
		.soil_density_kg_m3 = { 0.345 },
		.n_soil_adds = { 1 },
		.n_atm_adds = { 0 },
		.soil_additives = add_code_series_1,
		.atm_additives = NULL,
		.repeated = { 1 }
	};
	iter2 = (series_t) {
		.light_exposure = get_real_inline_matrix(10, 20),
		.soil_temp_c = get_real_inline_matrix(10, 2),
		.env_temp_c = get_real_array(10),
		.water_use_ml = get_real_array(10),
		.pH = 7,
		.p_bar = { 0.4567 },
		.soil_density_kg_m3 = { 678.345 },
		.n_soil_adds = { 1 },
		.n_atm_adds = { 0 },
		.soil_additives = add_code_series_2,
		.atm_additives = NULL,
		.repeated = { 3 }
	};
	
	series = malloc(2 * sizeof(series_t));
	*series = iter1;
	*(series + 1) = iter2;

	return series;
}

adf_t get_default_object(void)
{
	uint_t *codes = malloc(sizeof(uint_t));
	*codes = (uint_t){ 2345 };
	return (adf_t) {
		.header = (adf_header_t) { 
			.signature = { __ADF_SIGNATURE__ },
			.version = { __ADF_VERSION__ },
			.farming_tec = 3,
			.n_wavelength = { 20},
			.min_w_len_nm = { 0 },
			.max_w_len_nm = { 10000 },
			.n_depth = { 2 },
			.min_soil_depth_mm = { 0 },
			.max_soil_depth_mm = { 20 },
			.n_chunks = { 10 }
		},
		.metadata = (adf_meta_t) { 
			.period_sec = { 1345 },
			.n_additives = { 1 },
			.size_series = { 2 },
			.seeded = { 0 },
			.harvested = { 1345 },
			.n_series = 4,
			.additive_codes = codes
		},
		.series = get_default_series()
	};
}
