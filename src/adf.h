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

#ifndef __ADF_H__
#define __ADF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif /* __EMSCRIPTEN__ */

#define __ADF_SIGNATURE__ 0x40414446
#define __ADF_VERSION__ 0x01

typedef enum code {

	/*
	 * No errors detected
	 */
	OK = 0x00,

	/*
	 *
	 */
	HEADER_CORRUPTED = 0x01,
	/*
	 *
	 */
	SERIES_CORRUPTED = 0x02,

	/*
	 * The most generic error code.
	 */
	RUNTIME_ERROR = 1000
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
	 * pH of the soil. The data is stored as an 8-byte integer.
	 */
	uint8_t pH;

	/*
	 * Atmosferic pressure measured in bar.
	 */
	real_t p_bar;

	/*
	 *
	 */
	real_t soil_density_kg_m3;

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

	/*
	 *
	 */
	uint_small_t crc;
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
	uint8_t farming_tec;

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
} adf_header_t;

/*
 * The structure that contains all the data referred to a
 * fixed period of time.
 */
typedef struct {

	/*
	 *
	 */
	adf_header_t header;

	/*
	 *
	 */
	adf_meta_t metadata;

	/*
	 * The array of the iterations of size `n_iterations`.
	 * If n_series == 0, then iterations is NULL.
	 */
	series_t *series;
} __attribute__((packed)) adf_t;

/*
 * Returns the current version of ADF.
 */
// #ifdef __EMSCRIPTEN__
// EMSCRIPTEN_KEEPALIVE
// #endif /* __EMSCRIPTEN__ */
// int get_version(void) { return __ADF_VERSION__; }

// #ifdef __EMSCRIPTEN__
// EMSCRIPTEN_KEEPALIVE
// #endif /* __EMSCRIPTEN__ */
// adf_t get_adf(void) { return (adf_t){.signature = {__ADF_SIGNATURE__}}; }

/*
 * All iterations have the same size, as the series all have
 * the same length
 */
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif /* __EMSCRIPTEN__ */
size_t size_series_t(uint32_t, series_t);

/*
 *
 */
size_t size_medatata_t(adf_meta_t);

/*
 *
 */
size_t size_header(void);

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
long add_multiple_series(adf_t *, const series_t *, size_t);

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

#ifdef __cplusplus
}
#endif

#endif /* __ADF_H__ */
