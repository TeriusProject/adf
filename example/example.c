/*
 * example.c
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

#include <adf.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OUT_FILE_PATH "output.adf"
#define N_CHUNKS 10
#define N_WAVELENGTH 10
#define N_DEPTH 3

void write_file(adf_t adf)
{
	FILE *output_file;
	uint8_t *bytes;
	uint16_t res;
	size_t adf_size_bytes = size_adf_t(&adf);

	bytes = malloc(adf_size_bytes);
	if ((res = marshal(bytes, &adf)) != ADF_OK) {
		printf("An error occurred during marshal process [code: %x]\n", res);
		exit(1);
	}
	output_file = fopen(OUT_FILE_PATH, "wb");
	if (!output_file) {
		printf("Error: cannot create `%s` file.\n", OUT_FILE_PATH);
		exit(1);
	}
	fwrite(bytes, 1, size_adf_t(&adf), output_file);
	fclose(output_file);
	printf("Wrote %'lu bytes\n", adf_size_bytes);
	free(bytes);
}

/*
 * !!! Just a mock series with some random values !!!
 * In a real-world scenario, those series should be filled with data coming
 * from an agricolture database or - if you have a tech greenhouse - from 
 * your own sensors.
 */
series_t get_series(uint32_t n_chunks, uint16_t n_wavelength, uint16_t n_depth)
{
	series_t series;
	additive_t sample_additive;

	sample_additive = (additive_t) {
		.code = {1234},
		.code_idx = {0},
		.concentration = {123.456}
	};
	series.pH = rand() % 0xFFu;
	series.p_bar.val = (float)rand()/(float)(RAND_MAX);
	series.soil_density_kg_m3.val = (float)rand()/(float)(RAND_MAX);
	/* Repeated should never be less that 1 */
	series.repeated.val = 1 + (rand() % 10); 
	series.n_soil_adds.val = 1;
	series.n_atm_adds.val = 1;

	series.light_exposure = malloc(n_chunks * n_wavelength
								   * sizeof(real_t *));
	series.soil_temp_c = malloc(n_chunks * n_depth * sizeof(real_t *));
	series.env_temp_c = malloc(n_chunks * sizeof(real_t));
	series.water_use_ml = malloc(n_chunks * sizeof(real_t));
	series.soil_additives = malloc(sizeof(additive_t));
	series.atm_additives = malloc(sizeof(additive_t));

	for (uint32_t i = 0; i < n_chunks; i++) {
		series.env_temp_c[i].val = (float)rand()/(float)(RAND_MAX);
		series.water_use_ml[i].val = (float)rand()/(float)(RAND_MAX);

		for (uint16_t j = 0; j < n_wavelength; j++) 
			series.light_exposure[i].val = (float)rand()/(float)(RAND_MAX);
		for (uint16_t j = 0; j < n_depth; j++) 
			series.soil_temp_c[i].val = (float)rand()/(float)(RAND_MAX);

	}

	*series.soil_additives = sample_additive;
	*series.atm_additives = sample_additive;

	return series;
}

void register_data(adf_t *adf)
{
	series_t series_to_add;
	uint16_t res,
			 n_wave = adf->header.wave_info.n_wavelength.val,
			 n_depth = adf->header.soil_info.n_depth.val;
	for (uint16_t i = 1, n = rand() % 10'000; i <= n; i++) {
		printf("Adding series %d/%d\n", i, n);
		series_to_add = get_series(adf->header.n_chunks.val, 
								   n_wave,
								   n_depth);
		res = add_series(adf, &series_to_add);
		if (res != ADF_OK) {
			printf("An error occurred while adding the series [code:%x]", res);
			exit(1);
		}
	}
}

int main(void)
{
	adf_t adf;
	adf_header_t header;
	wavelength_info_t w_info;
	soil_depth_info_t s_info;
	reduction_info_t r_info = { 0 }; // Init to 0
	version_t version = get_adf_version();

	w_info = (wavelength_info_t) {
		.min_w_len_nm = { 0 },
		.max_w_len_nm = { 10000 },
		.n_wavelength = { N_WAVELENGTH },
	};

	s_info = (soil_depth_info_t) {
		.t_y = { 0 },
		.max_soil_depth_mm = { 300 },
		.n_depth = { N_DEPTH },
	};

	header = create_header(ADF_FT_REGULAR, w_info, s_info, r_info, N_CHUNKS);
	adf_init(&adf, header, ADF_DAY); // each series takes 1 day

	srand(time(NULL));
	setlocale(LC_ALL, "");
	
	printf("ADF version %d.%d.%d\n", version.major, version.minor, version.patch);
	register_data(&adf);
	printf("Writing on file `%s`...\n", OUT_FILE_PATH);
	write_file(adf);
	adf_free(&adf);
	printf("*DONE*\n");
}
