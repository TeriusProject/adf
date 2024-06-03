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

#include <stdio.h>
#include <stdlib.h>
#include "../src/adf.h"
#include "utils.h"

#define FILE_PATH "sample.adf"

real_t *get_light_mask()
{
	real_t *light_mask = malloc(10 * sizeof(real_t));
	float f = 0.0;
	for (int i = 0; i < 10; i++, f += 0.25)
		light_mask[i].val = f;
	return light_mask;
}

real_t *get_temp_celsius()
{
	return get_light_mask();
}

uint8_t *get_light_wavelengths()
{
	uint8_t *light_wavelengths = malloc(10 * sizeof(uint8_t));
	for (uint8_t i = 0; i < 10; i++)
		light_wavelengths[i] = i;
	return light_wavelengths;
}

real_t *get_water_use()
{
	return get_light_mask();
}

void print_real_array(real_t *arr, uint8_t dim)
{
	for (uint8_t i = 0; i < dim; i++) {
		printf("\t%d -> %f\n", i, arr[i].val);
	}
}

void print_int_array(uint8_t *arr, uint8_t dim)
{
	for (uint8_t i = 0; i < dim; i++) {
		printf("\t%d -> %d\n", i, arr[i]);
	}
}

int main()
{
	iter_t iter1 = {
		.light_exposure = get_light_mask(),
		.temp_celsius = get_temp_celsius(),
		.light_wavelength = get_light_wavelengths(),
		.water_use_ml = get_water_use(),
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
		.light_exposure = get_light_mask(),
		.temp_celsius = get_temp_celsius(),
		.light_wavelength = get_light_wavelengths(),
		.water_use_ml = get_water_use(),
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
		.signature = __ADF_SIGNATURE__,
		.version = __ADF_VERSION__,
		.min_w_len_nm = {0},
		.max_w_len_nm = {10000},
		.period = {1245637},
		.n_chunks = {10},
		.n_wavelength = {10},
		.n_iterations = {2},
		.iterations = iterations
	};
	uint8_t *bytes;
	FILE *sample_file = fopen(FILE_PATH, "rb");
	long file_len;

	if (sample_file == NULL) {
		printf("The file `%s` is not opened.", FILE_PATH);
		exit(0);
	}
	fseek(sample_file, 0, SEEK_END);
	file_len = ftell(sample_file);
	rewind(sample_file);

	bytes = (uint8_t *)malloc(file_len * sizeof(uint8_t));
	fread(bytes, file_len, 1, sample_file);
	fclose(sample_file);

	printf("bytes: %p\n", bytes);
	adf_t *new = unmarshal(bytes);
	if (!new) {
		printf("%s", "An error occurred during unmarshal process\n");
		return 1;
	}

	assert_true(new->signature == format.signature, "are signatures equals");
	assert_true(new->version == format.version, "are versions equals");
	assert_int_equal(new->n_wavelength, format.n_wavelength,"are n_wavelengths equal");
	assert_int_equal(new->min_w_len_nm, format.min_w_len_nm,"are min_w_len_nms equal");
	assert_int_equal(new->max_w_len_nm, format.max_w_len_nm,"are max_w_len_nms equal");
	assert_int_equal(new->period, format.period,"are periods equal");
	assert_int_equal(new->n_chunks, format.n_chunks,"are n_chunks equal");
	assert_int_equal(new->n_iterations, format.n_iterations,"are n_iterations equal");
}

// printf("new: %p\n", new);
// printf("Size: %zu bytes\n", adf_size(*new));
// printf("signature: %x\n", new->signature);
// printf("version: %x\n", new->version);
// printf("n: %d\n", new->n_chunks.val);
// printf("n_wavelength: %d\n", new->n_wavelength.val);
// printf("n_iterations: %d\n", new->n_iterations.val);
// for (uint32_t i = 0; i < new->n_iterations.val; i++) {
// 	iter_t iteration = new->iterations[i];
// 	printf("(iteration %d)\n", i);
// 	printf("%s:\n", "light mask");
// 	print_real_array(iteration.light_exposure, new->n_chunks.val);
// 	printf("%s:\n", "temperature");
// 	print_real_array(iteration.temp_celsius, new->n_chunks.val);
// 	printf("%s:\n", "water use");
// 	print_real_array(iteration.water_use_ml, new->n_chunks.val);
// 	printf("%s:\n", "light wavelength");
// 	print_int_array(iteration.light_wavelength, new->n_wavelength.val);
// 	printf("%s: %f\n", "pH", iteration.pH.val);
// 	printf("%s: %f\n", "pressure_pa", iteration.pressure_pa.val);
// 	printf("%s: %f\n", "soil_density_t_m3", iteration.soil_density_t_m3.val);
// 	printf("%s: %f\n", "nitrogen_g_m3", iteration.nitrogen_g_m3.val);
// 	printf("%s: %f\n", "potassium_g_m3", iteration.potassium_g_m3.val);
// 	printf("%s: %f\n", "phosphorus_g_m3", iteration.phosphorus_g_m3.val);
// 	printf("%s: %f\n", "iron_g_m3", iteration.iron_g_m3.val);
// 	printf("%s: %f\n", "magnesium_g_m3", iteration.magnesium_g_m3.val);
// 	printf("%s: %f\n", "sulfur_g_m3", iteration.sulfur_g_m3.val);
// 	printf("%s: %f\n", "calcium_g_m3", iteration.calcium_g_m3.val);
// }
