/*
 * adf.h
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

#ifndef __ADF_H__
#define __ADF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* The (hex) bytes of `@ADF` */
#define __ADF_SIGNATURE__ 0x40414446u

/*
 * Version is an unsigned int between 0 and 255. Versions are numbered in
 * progressive way, without any distinction between major and minor releases.
 */
#define __ADF_VERSION__ 0x01u

/* 
 * Used for the comparison of floating point numbers: numbers that have the 
 * first three decimals equal, are considered equals.
 */
#define EPSILON 1e-3

/*
 * It contains the exit code of the functions that handle the adf_t structure.
 */
typedef enum {
	/*
	 * Everything's OK.
	 */
	ADF_OK = 0x00u,

	/*
	 * Returned during unmarshalling, when the checksum included in the last
	 * two bytes of the header doesn't match with that calculated on the fly.
	 */
	ADF_HEADER_CORRUPTED = 0x01u,

	/*
	 * Returned during unmarshalling, when the checksum included in the last
	 * two bytes of the metadata doesn't match with that calculated on the fly.
	 */
	ADF_METADATA_CORRUPTED = 0x02u,

	/*
	 * Returned during unmarshalling, when the checksum included in the last
	 * two bytes of the header doesn't match with that calculated on the fly.
	 */
	ADF_SERIES_CORRUPTED = 0x03u,

	/*
	 * Returned when the field `repeated` in type series_t is set to 0.
	 */
	ADF_ZERO_REPEATED_SERIES = 0x04u,

	/*
	 * Returned in the delete_series function, when you are trying to delete
	 * a series, but the collection is empty.
	 */
	ADF_EMPTY_SERIES = 0x05u,

	/*
	 * Returned when you are trying to select a series (tipically for update),
	 * by specifying a time (in seconds) that is greater than the `period_sec`
	 * contained in the metadata section.
	 */
	ADF_TIME_OUT_OF_BOUND = 0x06u,

	/*
	 * 
	 */
	ADF_ADDITIVE_OVERFLOW = 0x07u,

	/*
	 * Error raised when a NULL pointer is passed as source in cpy_adf_header.
	 */
	ADF_NULL_HEADER_SOURCE = 0x08u,

	/*
	 * Error raised when a NULL pointer is passed as target in cpy_adf_header.
	 */
	ADF_NULL_HEADER_TARGET = 0x09u,

	/*
	 * Error raised when a NULL pointer is passed as source in cpy_adf_metadata.
	 */
	ADF_NULL_META_SOURCE = 0x0Au,

	/*
	 * Error raised when a NULL pointer is passed as target in cpy_adf_metadata.
	 */
	ADF_NULL_META_TARGET = 0x0Bu,

	/*
	 * Error raised when a NULL pointer is passed as source in cpy_adf_series.
	 */
	ADF_NULL_SERIES_SOURCE = 0x0Cu,

	/*
	 * Error raised when a NULL pointer is passed as target in cpy_adf_series.
	 */
	ADF_NULL_SERIES_TARGET = 0x0Du,

	/*
	 * Error raised when a NULL pointer is passed as source in cpy_adf.
	 */
	ADF_NULL_SOURCE = 0x0Eu,

	/*
	 * Error raised when a NULL pointer is passed as target in cpy_adf.
	 */
	ADF_NULL_TARGET = 0x0Fu,

	/*
	 * The most generic error code.
	 */
	ADF_RUNTIME_ERROR = 0xFFFFu
} code_t;

typedef union {
	float val;
	uint8_t bytes[4];
} real_t;

typedef union {
	uint32_t val;
	uint8_t bytes[4];
} uint_t;

typedef union {
	uint16_t val;
	uint8_t bytes[4];
} uint_small_t;

typedef struct {

	/*
	 * The index is the 4-byte unsigned index of the additive in
	 * metadata.additives
	 */
	uint_small_t code_idx;

	/*
	 * This field won't be serialized.
	 */
	uint_t code;
	
	/* the concentration in mg/kg */
	real_t concentration;
} additive_t;

/*
 * A structure that contains the data series.
 */
typedef struct {

	/*
	 * It represents the series of data collecting the energy flux
	 * of light radiation measured in W/m2 and divided in n_chunks.
	 */
	real_t *light_exposure;

	/*
	 * It represents the series of data collecting the temperature
	 * measured in ºC and divided in n_chunks.
	 */
	real_t *temp_celsius;

	/*
	 * It represents the series of data collecting the water use
	 * measured in milliliters and divided in n_chunks.
	 */
	real_t *water_use_ml;

	/*
	 * *Average* pH of the soil. The data is stored as an 8-byte integer.
	 */
	uint8_t pH;

	/*
	 * *Average* atmosferic pressure measured in bar.
	 */
	real_t p_bar;

	/*
	 * *Average* soil density measured in kg/m3
	 */
	real_t soil_density_kg_m3;

	/*
	 * The number of elements of the array `soil_additives`.
	 */
	uint_small_t n_soil_adds;

	/*
	 * The number of elements of the array `atm_additives`.
	 */
	uint_small_t n_atm_adds;

	additive_t *soil_additives;
	additive_t *atm_additives;

	/*
	 * The number of times this series is repeated consecutively. The number 0
	 * is not allowed. Set this field to 0 cause the ZERO_REPEATED_SERIES to
	 * be returned.
	 */
	uint_t repeated;
} __attribute__((packed)) series_t;

typedef struct {
	/*
	 * The number of series registered. The number 0 is used to
	 * indicate that there are no series. In that case, the `series`
	 * array could be NULL.
	 */
	uint_t size_series;

	/*
	 * The main difference between size_series and n_series is that
	 * size_series contains the size of the array `series`, while n_series
	 * contains the overall number of series (counting duplicates as well).
	 * This field won't be marshalled; it's computed on the fly during the
	 * unmarshalling procedure.
	 */
	uint32_t n_series;

	/*
	 * It represents the time (measured in seconds) that each series last. The
	 * total time elapsed (in seconds) is given by the period multiplied by
	 * the number of the series, including each repetition. Each series can last
	 * up to 65,535 seconds (approx. 18 hours)
	 */
	uint_small_t period_sec;

	/*
	 * Contains the number of elements of the array `additive_codes`.
	 */
	uint_small_t n_additives;

	/*
	 * Contains the unique code of each additive present in the series. Each
	 * additive should appear just once.
	 */
	uint_t *additive_codes;
} __attribute__((packed)) adf_meta_t;

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
	 * The lower bound of the light spectrum.
	 */
	uint_t min_w_len_nm;

	/*
	 * The upper bound of the light spectrum.
	 */
	uint_t max_w_len_nm;

	/*
	 * A 4 byte unsigned integer that represents the number of steps in which
	 * the light spectrum is (equally) divided. The spectrum represented here
	 * is bounded between:
	 *         [min_w_len_nm, max_w_len_nm]
	 */
	uint_t n_wavelength;

	/*
	 * The number of chunks in which each data series is (equally) divided.
	 */
	uint_t n_chunks;
} __attribute__((packed)) adf_header_t;

/*
 * The structure that contains all the ADF data.
 */
typedef struct {
	adf_header_t header;
	adf_meta_t metadata;

	/*
	 * The array of the iterations of size `n_iterations`.
	 * If size_series == 0, then iterations is NULL.
	 */
	series_t *series;
} __attribute__((packed)) adf_t;

/*
 * Returns the current version of ADF.
 */
uint8_t get_version(void);

/*
 * The size (bytes) of the adf header, including its crc field.
 * IMPORTANT: This is *not* the size of the struct adf_header_t; this is the
 * size of the serialized header data. The actual size in memory of the
 * adf_header_t structure may be bigger, due to some redundant fields that
 * speed up the mashalling and unmarshalling process.
 */
size_t size_header(void);

/*
 * The size (bytes) of the adf metadata section, including its crc field.
 * IMPORTANT: This is *not* the size of the struct adf_meta_t; this is the
 * size of the serialized metadata section. The actual size in memory of the
 * adf_meta_t structure may be bigger, due to some redundant fields that
 * speed up the mashalling and unmarshalling process.
 */
size_t size_medatata_t(adf_meta_t);

/*
 * The size (bytes) of *one* adf series, including the crc field. It takes the
 * number of chunks (i.e. the number of iterations in which some measures are
 * taken in the series) as the first parameter, and a series as the second.
 * Since all the series have the same size, we can get the size of all the
 * series block by multiplying this value with the field `size_series` in
 * metadata.
 * IMPORTANT: This is *not* the size of the struct series_t; this is the
 * size of each serialized series. The actual size in memory of the series_t
 * structure may be bigger, due to some redundant fields that speed up the
 * mashalling and unmarshalling process.
 */
size_t size_series_t(uint32_t, series_t);

/*
 * The size (bytes) of the adf object, including all the crc fields.
 * IMPORTANT: This is *not* the size of the struct adf_t; this is the size of
 * the serialized object as a whole. The actual size in memory of the adf_t
 * structure may be bigger, due to some redundant fields that speed up
 * the mashalling and unmarshalling process.
 */
size_t size_adf_t(adf_t);

/*
 * It returns a pointer to a chunk of memory that could contain the bytes
 * serialization of an adf object. The size of the returned byte array is
 * `size_adf_t`.
 */
uint8_t *adf_bytes_alloc(adf_t);

/*
 * 
 */
uint16_t add_series(adf_t *, series_t);

/*
 *
 */
uint16_t remove_series(adf_t *);

/*
 * Assumes the byte array `uint8_t *` to be allocated. You can get the exact
 * byte size to be allocated by the function `size_adf_t`. Alternatively, you
 * can allocate it directly with `bytes_alloc`. 
 */
uint16_t marshal(uint8_t *, adf_t);

/*
 * Assumes the `adf_t *` structure to be allocated.
 */
uint16_t unmarshal(adf_t *, const uint8_t *);

/*
 * It updates the series at time `uint64_t`.
 * It considers the repetitions as well.
 */
uint16_t update_series(adf_t *, series_t, uint64_t);

/*
 *
 */
uint16_t reindex_additives(adf_t *);

bool are_series_equal(series_t, series_t, uint32_t);
adf_header_t create_header(uint8_t, uint32_t, uint32_t, uint32_t, uint32_t);
adf_meta_t create_metadata(uint32_t *, uint16_t, uint32_t, uint32_t, uint16_t);
adf_t create_adf(adf_header_t, adf_meta_t);
adf_t create_empty_adf(adf_header_t);
void metadata_free(adf_meta_t *);
void series_free(series_t *);
void adf_free(adf_t *);
uint16_t cpy_adf(adf_t *, const adf_t *);
uint16_t cpy_adf_header(adf_header_t *, const adf_header_t *);
uint16_t cpy_adf_metadata(adf_meta_t *, const adf_meta_t *);
uint16_t cpy_adf_series(series_t *, const series_t *, uint32_t, uint32_t);

#ifdef __cplusplus
}
#endif

#endif /* __ADF_H__ */
