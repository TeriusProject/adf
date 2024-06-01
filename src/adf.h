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

#ifndef __ADF__
#define __ADF__

#include <stdint.h>
#include <stdlib.h>

#define SIGNATURE 0xAD    /* 173 */

typedef union real {
	float val;
	uint8_t bytes[4];
} real_t;

/*
 *
 */
typedef struct {
	real_t *light_mask;
	uint8_t *light_wavelenght;
	real_t *temperatures_centigrades;
	real_t pH;
	real_t pressure_pa;
	uint8_t *water_use_ml;
	real_t soil_density_t_m3;
	real_t nitrogen_g_m3;
	real_t potassium_g_m3;
	real_t phosphorus_g_m3;
	real_t iron_g_m3;
	real_t magnesium_g_m3;
	real_t sulfur_g_m3;
	real_t calcium_g_m3;
} __attribute__((packed)) iter_t;

/*
 *
 */
typedef struct {
	uint8_t signature;
	uint8_t n;
	uint8_t wavelength_n;
	uint8_t n_iterations;
	iter_t *iterations;
} __attribute__((packed)) adf_t;

/*
 *
 */
size_t adf_size(adf_t);

/*
 *
 */
uint8_t *marshal(adf_t);

/*
 *
 */
adf_t *unmarshal(const uint8_t *);

/*
 *
 */
void append_iteration(adf_t *, iter_t);

#endif /* __ADF__ */
