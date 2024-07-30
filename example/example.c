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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define OUT_FILE_PATH "output.adf"

void write_file(adf_t adf)
{
	FILE *output_file;
	uint8_t *bytes;
	uint16_t res;

	bytes = adf_bytes_alloc(adf);
	if ((res = marshal(bytes, adf)) != ADF_OK) {
		printf("An error occurred during marshal process [code: %x]\n", res);
		exit(1);
	}
	output_file = fopen(OUT_FILE_PATH, "wb");
	if (!output_file) {
		perror("Error: ");
		exit(1);
	}
	fwrite(bytes, 1, size_adf_t(adf), output_file);
	fclose(output_file);
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

	series.light_exposure = malloc(n_chunks * sizeof(real_t *));
	series.soil_temp_c = malloc(n_chunks * sizeof(real_t *));
	series.env_temp_c = malloc(n_chunks * sizeof(real_t));
	series.water_use_ml = malloc(n_chunks * sizeof(real_t));
	series.soil_additives = malloc(sizeof(additive_t));
	series.atm_additives = malloc(sizeof(additive_t));

	for (uint32_t i = 0; i < n_chunks; i++) {
		series.env_temp_c[i].val = (float)rand()/(float)(RAND_MAX);
		series.water_use_ml[i].val = (float)rand()/(float)(RAND_MAX);
		series.light_exposure[i] = malloc(n_wavelength * sizeof(real_t));
		series.soil_temp_c[i] = malloc(n_depth * sizeof(real_t));

		for (uint16_t j = 0; j < n_wavelength; j++) 
			series.light_exposure[i][j].val = (float)rand()/(float)(RAND_MAX);
		for (uint16_t j = 0; j < n_depth; j++) 
			series.soil_temp_c[i][j].val = (float)rand()/(float)(RAND_MAX);

	}

	*series.soil_additives = sample_additive;
	*series.atm_additives = sample_additive;

	return series;
}

void register_data(adf_t *adf)
{
	uint16_t res;
	series_t series_to_add;

	for (uint16_t i = 1, n = rand() % 10'000; i <= n; i++) {
		printf("Adding series %d/%d\n", i, n);
		series_to_add = get_series(adf->header.n_chunks.val, 
								   adf->header.n_wavelength.val,
								   adf->header.n_depth.val);
		res = add_series(adf, &series_to_add);
		if (res != ADF_OK) {
			printf("An error occurred while adding the series [code:%x]", res);
			exit(1);
		}
	}
}

int main(void)
{
	adf_header_t header;
	adf_t adf;
	
	header = (adf_header_t) { 
		.signature = { __ADF_SIGNATURE__ },
		.version = { __ADF_VERSION__ },
		.farming_tec = 0x01u,
		.min_w_len_nm = { 0 },
		.max_w_len_nm = { 10000 },
		.n_chunks = { 10 },
		.n_wavelength = { 10 },
		.n_depth = { 3 },
		.min_soil_depth_mm = { 0 },
		.max_soil_depth_mm = { 300 }
	};
	adf_init(&adf, header, ADF_DAY); // each series takes 1 day

	srand(time(NULL));
	register_data(&adf);
	printf("Writing on file `%s`...\n", OUT_FILE_PATH);
	write_file(adf);
	adf_free(&adf);
	printf("*DONE*\n");
}
