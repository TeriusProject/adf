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

#define __ADF_SIGNATURE__ 0x40414446
#define __ADF_VERSION__ 0x01

typedef enum code {
	OK = 0,
	NOT_OK = 1
} code_t;

typedef union real {
	float val;
	uint8_t bytes[4];
} real_t;

typedef union uint {
	uint32_t val;
	uint8_t bytes[4];
} uint_t;

/*
 *
 */
typedef struct {

	/*
	*
	*/
	real_t *light_exposure;

	/*  */
	real_t *temp_celsius;

	/*  */
	real_t *water_use_ml;

	/*  */
	uint8_t *light_wavelength;

	/*  */
	real_t pH;

	/*  */
	real_t pressure_pa;

	/*  */
	real_t soil_density_t_m3;

	/*  */
	real_t nitrogen_g_m3;

	/*  */
	real_t potassium_g_m3;

	/*  */
	real_t phosphorus_g_m3;

	/*  */
	real_t iron_g_m3;

	/*  */
	real_t magnesium_g_m3;

	/*  */
	real_t sulfur_g_m3;

	/*  */
	real_t calcium_g_m3;
} __attribute__((packed)) iter_t;

/*
 *
 */
typedef struct {
	/*
	 * Signature contains the following four bytes
	 * 		0x40  0x41  0x44  0x46
	 * Those bytes are contained in the macro __ADF_SIGNATURE__
	 */
	uint_t signature;

	/*
	 * Version is an 8-bit unsigned integer that contains the
	 * (progressive) version of the format. Yhis byte is
	 * contained in the macro __ADF_VERSION__
	 */
	uint8_t version;

	/*
	 * 
	 */
	uint_t n_wavelength;

	/*  */
	uint_t min_w_len_nm;

	/*  */
	uint_t max_w_len_nm;

	/*  */
	uint_t period;

	/*  */
	uint_t n_chunks;

	/*  */
	uint_t n_iterations;

	/*  */
	iter_t *iterations;
} __attribute__((packed)) adf_t;

/*
 *
 */
size_t size_iter_t(adf_t data);

/*
 *
 */
size_t size_adf_t(adf_t);

/*
 *
 */
uint8_t *bytes_alloc(adf_t);

/*
 *
 */
long marshal(uint8_t *, adf_t);

/*
 *
 */
long unmarshal(adf_t *, const uint8_t *);

#endif /* __ADF__ */
