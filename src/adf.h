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
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif /* __EMSCRIPTEN__ */

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
 * A structure that contains the data series of each iteration
 */
typedef struct {

	/*
	 * It represents the series of data collecting the energy flux 
	 * of light radiation measured in W/m2 and divided in n_chunks
	 */
	real_t *light_exposure;

	/*
	 * It represents the series of data collecting the temperature 
	 * measured in ºC and divided in n_chunks
	 */
	real_t *temp_celsius;

	/*
	 * It represents the series of data collecting the water use 
	 * measured in milliliters and divided in n_chunks
	 */
	real_t *water_use_ml;

	/*
	 * 
	 */
	real_t *light_wavelength;

	/*
	 * pH of the soil measured once per iteration.
	 */
	real_t pH;

	/*
	 * Atmosferic pressure measured in Pascal, once per iteration.
	 */
	real_t pressure_pa;

	/*
	 * Soil density measured in t/m3, once per iteration.
	 */
	real_t soil_density_t_m3;

	/*
	 * Nitrogen concentration measured in g/m3, once per
	 * iteration.
	 */
	real_t nitrogen_g_m3;

	/*
	 * Potassium concentration measured in g/m3, once per
	 * iteration
	 */
	real_t potassium_g_m3;

	/*
	 * Phoshorus concentration measured in g/m3, once per
	 * iteration
	 */
	real_t phosphorus_g_m3;

	/*
	 * Iron concentration measured in g/m3, once per
	 * iteration
	 */
	real_t iron_g_m3;

	/*
	 * Magnesium concentration measured in g/m3, once per
	 * iteration
	 */
	real_t magnesium_g_m3;

	/*
	 * Sulfur concentration measured in g/m3, once per
	 * iteration
	 */
	real_t sulfur_g_m3;

	/*
	 * Calcium concentration measured in g/m3, once per
	 * iteration
	 */
	real_t calcium_g_m3;
} __attribute__((packed)) iter_t;

/*
 * The structure that contains all the data referred to a
 * fixed period of time.
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
	 * A 4 byte unsigned integer that represents the number
	 * of steps in which the light spectrum is divided. The
	 * spectrum represented here is bounded between:
	 *         [min_w_len_nm, max_w_len_nm]
	 */
	uint_t n_wavelength;

	/*
	 * The lower bound of the light spectrum
	 */
	uint_t min_w_len_nm;

	/*
	 * The upper bound of the light spectrum
	 */
	uint_t max_w_len_nm;

	/*
	 * It represents the time (measured in seconds) period to
	 * which the data refer.
	 *         0 <= period_sec <= 49,640 days
	 */
	uint_t period_sec;

	/*
	 * The number of chunks in which each data series is
	 * (equally) divided
	 */
	uint_t n_chunks;

	/*
	 * The number of iterations registered. 0 is allowed
	 */
	uint_t n_iterations;

	/*
	 * The array of the iterations of size `n_iterations`.
	 * If n_iterations == 0, then iterations is NULL.
	 */
	iter_t *iterations;
} __attribute__((packed)) adf_t;

/*
 * Returns the current version of ADF.
 */
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif /* __EMSCRIPTEN__ */
int get_version(void) { return __ADF_VERSION__; }






#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif /* __EMSCRIPTEN__ */
adf_t get_adf(void) { return (adf_t){.signature= __ADF_SIGNATURE__}; }









/*
 * All iterations have the same size, as the series all have 
 * the same length
 */
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif /* __EMSCRIPTEN__ */
size_t size_iter_t(adf_t);

/*
 * The size of the adf object
 */
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif /* __EMSCRIPTEN__ */
size_t size_adf_t(adf_t);

/*
 * It returns a pointer to a chunk of memory that could contain 
 * the bytes serialization of an adf object. 
 */
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif /* __EMSCRIPTEN__ */
uint8_t *bytes_alloc(adf_t);

/*
 *
 */
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif /* __EMSCRIPTEN__ */
long marshal(uint8_t *, adf_t);

/*
 *
 */
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif /* __EMSCRIPTEN__ */
long unmarshal(adf_t *, const uint8_t *);

#endif /* __ADF__ */
