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

	/* No errors detected */
	OK = 0,

	NOT_OK = 1000
} code_t;

typedef union real {
	float val;
	uint8_t bytes[4];
} real_t;

typedef union uint {
	uint32_t val;
	uint8_t bytes[4];
} uint_t;

typedef union usmallint {
	uint16_t val;
	uint8_t bytes[4];
} uint_small_t;

typedef struct {
	uint_small_t code;
	real_t concentration;
} additive_t;

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
	 * pH of the soil measured once per iteration.
	 */
	uint8_t pH;

	/*
	 * Atmosferic pressure measured in bar, once per iteration.
	 */
	real_t p_bar;

	/*
	 *
	 */
	real_t soil_density;

	/*
	 *
	 */
	uint_small_t n_soil_adds;

	/*
	 *
	 */
	uint_small_t n_atm_adds;

	/*
	 *
	 */
	additive_t *soil_additives;

	/*
	 *
	 */
	additive_t *atm_additives;

	/*
	 *
	 */
	uint_t repeated;
} __attribute__((packed)) series_t;

typedef struct {

	/*
	 * The number of iterations registered. 0 is allowed
	 */
	uint_t n_series;

	/*
	 * It represents the time (measured in seconds) period to
	 * which the data refer.
	 *         0 <= period_sec <= 49,640 days
	 */
	uint_t period_sec;

	/*
	 *
	 */
	uint_small_t n_additives;

	/*
	 *
	 */
	uint_t *additive_codes;
} adf_meta_t;

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
	 *
	 */
	uint8_t farming_tecnique;

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
	 * The number of chunks in which each data series is
	 * (equally) divided
	 */
	uint_t n_chunks;

	/*
	 *
	 */
	uint_small_t crc;

	/*
	 *
	 */
	adf_meta_t metadata;

	/*
	 * The array of the iterations of size `n_iterations`.
	 * If n_series == 0, then iterations is NULL.
	 */
	iter_t *series;
} __attribute__((packed)) adf_t;

/*
 * All iterations have the same size, as the series all have
 * the same length
 */
size_t size_iter_t(adf_t);

/*
 * The size of the adf object
 */
size_t size_adf_t(adf_t);

/*
 * It returns a pointer to a chunk of memory that could contain
 * the bytes serialization of an adf object.
 */
uint8_t *bytes_alloc(adf_t);

/*
 *
 */
long add_series(adf_t *, const series_t *);

/*
 *
 */
long marshal(uint8_t *, adf_t);

/*
 *
 */
long unmarshal(adf_t *, const uint8_t *);

#endif /* __ADF__ */
