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
 * Terius is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <adf.h>
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
}

void split_to_float_arr(char *arr_str, real_t *arr, uint32_t n)
{
	size_t idx = 0;
	char delim[] = " ";
	char *ptr = strtok(arr_str, delim);
	if (ptr) {
		arr[idx].val = (float) atof(ptr);
	}

	while (ptr) {
		idx++;
		if (idx >= n) {
			printf("Too many values, limit is %d\n", n);
			exit(1);
		}
		arr[idx].val = (float) atof(ptr);
		printf("%f\n",(float) atof(ptr));
		ptr = strtok(NULL, delim);
	}
}

/*
 * !!! Just a mock series with some random values !!!
 * In a real-world scenario, those series should be filled with data coming
 * from an agricolture database or - if you have a tech greenhouse - from 
 * your own sensors.
 */
series_t get_series(uint32_t n_chunks, uint32_t n_wavelength)
{
	series_t series;
	additive_t sample_additive = (additive_t) {
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

	series.light_exposure = malloc(n_wavelength * sizeof(real_t));
	series.temp_celsius = malloc(n_chunks * sizeof(real_t));
	series.water_use_ml = malloc(n_chunks * sizeof(real_t));
	series.soil_additives = malloc(sizeof(additive_t));
	series.atm_additives = malloc(sizeof(additive_t));

	for (uint32_t i = 0; i < n_chunks; i++) {
		series.temp_celsius[i].val = (float)rand()/(float)(RAND_MAX);
		series.water_use_ml[i].val = (float)rand()/(float)(RAND_MAX);
	}

	for (uint32_t i = 0; i < n_wavelength; i++)
		series.light_exposure[i].val = (float)rand()/(float)(RAND_MAX);

	*series.soil_additives = sample_additive;
	*series.atm_additives = sample_additive;

	return series;
}

void register_data(adf_t *adf)
{
	series_t series_to_add;
	uint16_t res;

	for (uint8_t i = 1, n = rand() % 0xFFu; i <= n; i++) {
		printf("Adding series %d/%d\n", i, n);
		series_to_add = get_series(adf->header.n_chunks.val, 
								   adf->header.n_wavelength.val);
		res = add_series(adf,series_to_add);
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
		.version = __ADF_VERSION__,
		.farming_tec = 0x01u,
		.min_w_len_nm = { 0 },
		.max_w_len_nm = { 10000 },
		.n_chunks = { 10 },
		.n_wavelength = { 10 } 
	};
	adf_init(&adf, header, 86400); // each series takes 1 day

	srand(time(NULL));
	register_data(&adf);
	printf("Writing on file `%s`...\n", OUT_FILE_PATH);
	write_file(adf);
	printf("*DONE*\n");
}
