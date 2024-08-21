/* adf.h
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
 * Version is an unsigned 2-byte integer.
 * The most significant byte represents the major release. The first half of
 * the least significant byte, the minor release, while the second half the
 * patch release.
 *
 * Example
 * -------
 * If the constant __ADF_VERSION__ hase the value 0x01A1u, it has to be
 * parsed as follows
 *     Major version -> 0x01
 *     Minor version -> 0xA
 *     Patch version -> 0x1
 * So, this ADF version is 1.10.1
 */
#define __ADF_VERSION__ 0x0092u
#define MAJOR_VERSION_MASK 0xFF00u
#define MINOR_VERSION_MASK 0x00F0u
#define PATCH_VERSION_MASK 0x000Fu

/*
 * Used for the comparison of floating point numbers: numbers that have the
 * first three decimals equal, are considered equals.
 */
#define EPSILON 1e-3

/*
 * The size of the datatypes used in the ADF format.
 */
#define UINT_BIG_T_SIZE    8
#define UINT_T_SIZE        4
#define UINT_SMALL_T_SIZE  2
#define UINT_TINY_T_SIZE   1
#define REAL_T_SIZE        4
#define ADD_T_SIZE         6

/*
 * Some constants to represent some of the most frequently used standard
 * duration of the series.
 * All numbers are expressed in seconds.
 */
#define ADF_DAY         86'400
#define ADF_WEEK        604'800
#define ADF_MONTH_28    2'419'200
#define ADF_MONTH_29    2'505'600
#define ADF_MONTH_30    2'592'000
#define ADF_MONTH_31    2'678'400

/*
 * The most significant byte contains the parent category, while the least
 * significant the subcategory. Thence, there are up to 16 subcategories
 * for every parent category.
 *
 *     + 0x00 ADF_FT_REGULAR
 *       - 0x01 ADF_FT_INDOOR
 *       - 0x02 ADF_FT_INDOOR_PROTECTED
 *       - ...
 *     + ...
 */
typedef enum {
	ADF_FT_REGULAR            = 0x00u,
	ADF_FT_INDOOR             = 0x01u,
	ADF_FT_INDOOR_PROTECTED   = 0x02u,
	ADF_FT_OUTDOOR            = 0x03u,
	ADF_FT_ARTIFICIAL_SOIL    = 0x10u,
	ADF_FT_HYDROPONICS        = 0x20u,
	ADF_FT_ANTHROPONICS       = 0x21u,
	ADF_FT_AEROPONICS         = 0x30u,
	ADF_FT_FOGPONICS          = 0x31u
} farming_technique_t;

/*
 * 
 */
typedef struct {
	uint8_t major;
	uint8_t minor;
	uint8_t patch;
} version_t;

/*
 * 8-bit usnsigned integer that represents the statistical procedure applied
 * to reduce the data within the series
 */
typedef enum {

	/* No statistical procedure has been applied */
	ADF_RM_NONE = 0x00u,

	/* Average */
	ADF_RM_AVG  = 0x01u,

	/* Moving average */
	ADF_RM_MAVG = 0x02u
} reduction_code_t;

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
	 * Returned when you are trying to add or aupdate a series with the field
	 * `repeated` set to 0.
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
	 * You are trying to add a series with additives that are not already
	 * present in metadata's additive_codes array. You reached the limit
	 * of that array though (0xFFFF).
	 */
	ADF_ADDITIVE_OVERFLOW = 0x07u,

	/* Error raised when NULL is passed as source in cpy_adf_header. */
	ADF_NULL_HEADER_SOURCE = 0x08u,

	/* Error raised when NULL is passed as target in cpy_adf_header. */
	ADF_NULL_HEADER_TARGET = 0x09u,

	/* Error raised when NULL is passed as source in cpy_adf_metadata. */
	ADF_NULL_META_SOURCE = 0x0Au,

	/* Error raised when NULL is passed as target in cpy_adf_metadata. */
	ADF_NULL_META_TARGET = 0x0Bu,

	/* Error raised when  NULL is passed as source in cpy_adf_series. */
	ADF_NULL_SERIES_SOURCE = 0x0Cu,

	/* Error raised when NULL is passed as target in cpy_adf_series. */
	ADF_NULL_SERIES_TARGET = 0x0Du,

	/* Error raised when NULL is passed as source in cpy_adf. */
	ADF_NULL_SOURCE = 0x0Eu,

	/* Error raised when NULL is passed as target in cpy_adf. */
	ADF_NULL_TARGET = 0x0Fu,

	/* Error raised when NULL is passed as source in cpy_series. */
	ADF_NULL_ADDITIVE_SOURCE = 0x10u,

	/* Error raised when NULL is passed as target in cpy_series. */
	ADF_NULL_ADDITIVE_TARGET = 0x11u,

	/* The most generic error code. */
	ADF_RUNTIME_ERROR = 0xFFFFu
} code_t;

/*
 * Those strings are used by the APIs to get standard error messages.
 */
#define ADF_ERROR_PREFIX "-- ADF ERROR -- "
#define ADF_HEADER_CORRUPTED_STR "Header is corrupted. Cannot unmarshal"
#define ADF_METADATA_CORRUPTED_STR "Metadata is corrupted. Cannot unmarshal"
#define ADF_SERIES_CORRUPTED_STR "A series is corrupted. Cannot unmarshal"
#define ADF_ZERO_REPEATED_SERIES_STR "Cannot add/update a series repeated 0 " \
									 "times"
#define ADF_EMPTY_SERIES_STR "Cannot remove a series. The series collection " \
							 "is empty"
#define ADF_TIME_OUT_OF_BOUND_STR "Cannot update the series. the time you "   \
								  "specified is out of bound"
#define ADF_ADDITIVE_OVERFLOW_STR "Cannot add/update the series. Yhe number " \
								  "of additives present in this ADF file is " \
								  "greter than 65'535"
#define ADF_NULL_HEADER_SOURCE_STR "Cannot copy: source header is NULL"
#define ADF_NULL_HEADER_TARGET_STR "Cannot copy: target header is NULL"
#define ADF_NULL_META_SOURCE_STR "Cannot copy: source metadata is NULL"
#define ADF_NULL_META_TARGET_STR "Cannot copy: target metadata is NULL"
#define ADF_NULL_SERIES_SOURCE_STR "Cannot copy: source series is NULL"
#define ADF_NULL_SERIES_TARGET_STR "Cannot copy: target series is NULL"
#define ADF_NULL_SOURCE_STR "Cannot copy: source is NULL"
#define ADF_NULL_TARGET_STR "Cannot copy: target is NULL"
#define ADF_NULL_ADDITIVE_SOURCE_STR "Cannot copy: source additive is NULL"
#define ADF_NULL_ADDITIVE_TARGET_STR "Cannot copy: target additive is NULL"
#define ADF_RUNTIME_ERROR_STR "An error occurred"

typedef union {
	float val;
	uint8_t bytes[4];
} real_t;

typedef union {
	uint64_t val;
	uint8_t bytes[8];
} uint_big_t;

typedef union {
	uint32_t val;
	uint8_t bytes[4];
} uint_t;

typedef union {
	uint16_t val;
	uint8_t bytes[2];
} uint_small_t;

/*
 * A structure that contains the concentration of each additive included
 * in each series
 */
typedef struct {

	/*
	 * The 4-byte unsigned index of the additive in metadata.additives.
	 * This field *should never* be populated by hand. It's automatically filled
	 * by the function `add_series`.
	 */
	uint_small_t code_idx;

	/*
	 * This field won't be serialized into the binary file.
	 * This field *must* be populated when you pass an additive to a series.
	 */
	uint_t code;

	/*
	 * The concentration of this additive in the soil/atmosphere.
	 * It's expressed in mg/kg.
	 */
	real_t concentration;
} __attribute__(( packed )) additive_t;

/*
 * The structure that contains the data series.
 */
typedef struct {

	/*
	 * It represents the series of data collecting the energy flux
	 * of light radiation measured in W/m2 and divided in n_chunks.
	 */
	real_t *light_exposure;

	/*
	 * 
	 */
	real_t *soil_temp_c;

	/*
	 * It represents the series of data collecting the environment temperature
	 * measured in ºC and divided in n_chunks.
	 */
	real_t *env_temp_c;

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
	 * Pressure measured in bar. These data are reduced as specified in the
	 * `pressure_red_mode` field of the reduction_info conatined in the
	 * header section.
	 */
	real_t p_bar;

	/*
	 * Soil density measured in kg/m3. These data are reduced as specified
	 * in the `soil_density_red_mode` field of the reduction_info conatined in
	 * the header section.
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

	/*
	 * Each series can have its own additives. These two arrays does not
	 * contain directly the code of each additive, but its index on the
	 * additive_codes array in the metadata section.
	 */
	additive_t *soil_additives;
	additive_t *atm_additives;

	/*
	 * The number of times this series is repeated consecutively. The number 0
	 * is not allowed. Set this field to 0 cause the ZERO_REPEATED_SERIES to
	 * be returned.
	 */
	uint_t repeated;
} __attribute__(( packed )) series_t;

/*
 * It contains all mutable metadata of an adf struct. This metadata
 * typically changes whenever operations are performed on the series
 * (e.g., one is added or removed)
 */
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
	 * contains the overall number of series (including repeated ones).
	 * This field won't be marshalled; it's computed on the fly during the
	 * unmarshalling procedure.
	 * It should be a 64-bits integer, since it has to contain the number
	 *     (0xFFFFFFFF*0xFFFFFFFF)
	 */
	uint64_t n_series;

	/*
	 * It represents the time (measured in seconds) that each series last. The
	 * total time elapsed (in seconds) is given by the period multiplied by
	 * the number of the series, including each repetition. Each series can last
	 * up to 4,294,967,295 seconds
	 */
	uint_t period_sec;

	/*
	 * The time (measured in seconds) when the crops were seeded.
	 */
	uint_big_t seeded;

	/*
	 * The time (measured in seconds) when the crops were harvested.
	 */
	uint_big_t harvested;

	/*
	 * Contains the number of elements of the array `additive_codes`.
	 */
	uint_small_t n_additives;

	/*
	 * Contains the unique code of each additive present in the series. Each
	 * additive should appear just once.
	 */
	uint_t *additive_codes;
} __attribute__(( packed )) adf_meta_t;

typedef struct {

	/*
	 * The lower bound of the recorded light spectrum measured in nanometers.
	 */
	uint_small_t min_w_len_nm;

	/*
	 * The upper bound of the recorded light spectrum measured in nanometers.
	 */
	uint_small_t max_w_len_nm;

	/*
	 * A 2 byte unsigned integer that represents the number of sections in which
	 * the light spectrum is (equally) divided. The spectrum represented here
	 * is bounded between:
	 *         [min_w_len_nm, max_w_len_nm]
	 */
	uint_small_t n_wavelength;
} __attribute__(( packed )) wavelength_info_t;

/*
 * This structure contains information about the soil tempretature measurements.
 * In particular:
 *     - The maximum depth recorded;
 *     - The number of layers in which the soil has been divided;
 *     - A vertical translation coefficient.
 * Let's consider the case in which we have a translation = 0 (i.e. we start
 * measuring from the ground). We have something like this:
 *
 *     +-----+   <- t_y = 0 mm (ground)
 *     |     |
 *     |..o..|   <- a temperature sensor at x mm depth.
 *     |     |
 *     |..o..|   <- a temperature sensor at 2x mm depth.
 *     |     |      max_soil_depth_mm = 2x mm
 *     :  :  :
 *     :  :  :
 * The total length of the vertical section is l = (max_soil_depth_mm - 0). In
 * this case we have a n_depth of 2, that means that we have two layers. The
 * vertical length of each layer is x = l/2.
 *
 * !!! Note !!! Each sensor is placed at the bottom of its own layer
 * (as you can see in figure).
 *
 * Here we have the general case, in which a translation on t_y mm has been
 * applied. In this case, the total length is l = (max_soil_depth_mm - t_y),
 * and each layer has vertical length equal to l / n_depth.
 *
 *     +-----+
 *     |_ _ _|   <- t_y = 1 mm
 *     |     |
 *     |..o..|   <- first sensor placed at the bottom of the first layer
 *     |     |
 *     |..o..|   <- a temperature sensor at (2x + 1) mm depth.
 *     |     |
 *     :  :  :
 *     :  :  :
 */
typedef struct {

	/*
	 * The depth at which you can find the deepest temperature sensor.
	 */
	uint_small_t max_soil_depth_mm;

	/*
	 * The (possibly 0) translation on the (positive) Y axis of the shallowest
	 * sensor
	 */
	uint_small_t t_y;

	/* The number of layers in which the soil has been (vertically) divided */
	uint_small_t n_depth;
} __attribute__(( packed )) soil_depth_info_t;

/*
 * Each of the following fields contain information about the statistical
 * procedure used to reduce the data within the series.
 *     Default value: None (0x00u)
 *     Meaning: No statistical procedure has been applied.
 */
typedef struct {
	reduction_code_t soil_density_red_mode;
	reduction_code_t pressure_red_mode;
	reduction_code_t light_exposure_red_mode;
	reduction_code_t water_use_red_mode;
	reduction_code_t soil_temp_red_mode;
	reduction_code_t env_temp_red_mode;
} __attribute__(( packed )) reduction_info_t;

/*
 * Each field in this structure specifies the precision with which the
 * respective values within the series are measured.
 */
typedef struct {
	real_t soil_density_prec;
	real_t pressure_prec;
	real_t light_exposure_prec;
	real_t water_use_prec;
	real_t soil_temp_prec;
	real_t env_temp_prec;
	real_t additive_prec;
} __attribute__(( packed )) precision_info_t;

/*
 * This structure contains the immutable data of an ADF structure.
 * none of the operations executed on an ADF structure should change
 * these fields, according to the specifications of the ADF format.
 */
typedef struct {

	/*
	 * Signature contains the following four bytes

	 * 		0x40  0x41  0x44  0x46
	 * Those bytes are contained in the macro __ADF_SIGNATURE__.
	 */
	uint_t signature;

	/*
	 * Version is an 16-bit unsigned integer that contains the
	 * version of the ADF format. These bytes are contained in
	 * the macro __ADF_VERSION__.
	 */
	uint_small_t version;

	farming_technique_t farming_tec;
	wavelength_info_t wave_info;
	soil_depth_info_t soil_info;
	reduction_info_t reduction_info;
	precision_info_t precision_info;

	/*
	 * The number of chunks in which each data series is (equally) divided.
	 */
	uint_t n_chunks;
} __attribute__(( packed )) adf_header_t;

/*
 * The structure that contains all the ADF data.
 */
typedef struct {

	adf_header_t header;
	adf_meta_t metadata;

	/*
	 * The array of the iterations of size `n_iterations`.
	 * If size_series == 0, then this array is set to NULL.
	 */
	series_t *series;
} __attribute__(( packed )) adf_t;

/*
 * Returns the (unpacked) current version of ADF.
 */
version_t get_adf_version(void);

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
size_t size_medatata_t(adf_meta_t *);

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
size_t size_series_t(adf_t *, series_t *);

/*
 * The size (bytes) of the adf object, including all the crc fields.
 * IMPORTANT: This is *not* the size of the struct adf_t; this is the size of
 * the serialized object as a whole. The actual size in memory of the adf_t
 * structure is bigger, due to some redundant fields contained in it to help
 * speeding up the mashalling and unmarshalling process.
 */
size_t size_adf_t(adf_t *);

/*
 * It returns a pointer to a chunk of memory that could contain the bytes
 * serialization of an adf object. The size of the returned byte array is
 * `size_adf_t`.
 */
uint8_t *adf_bytes_alloc(adf_t *);

/* It frees the byte array */
void adf_bytes_free(uint8_t *);

/*
 * Adds a series to the adf structure.
 * If the series is equal to the last one contained in the `series` array
 * 
 */
uint16_t add_series(adf_t *, const series_t *);

/*
 * 
 */
uint16_t remove_series(adf_t *);

/*
 * 
 */
uint16_t reindex_additives(adf_t *);

/*
 *
 */
uint16_t get_series_at(adf_t *, series_t *, uint64_t);

/*
 * Assumes the byte array `uint8_t *` to be allocated. You can get the exact
 * byte size to be allocated by the function `size_adf_t`. Alternatively, you
 * can allocate it directly with `bytes_alloc`.
 */
uint16_t marshal(uint8_t *, adf_t *);

/* Assumes the adf_t structure not to be NULL. */
uint16_t unmarshal(adf_t *, const uint8_t *);

/* It updates the series at a certain time. */
uint16_t update_series(adf_t *, const series_t *, uint64_t);

/*
 * It overwrites the source series to those already contained in the adf struct.
 * !!! All the series contained in the adf before will be deleted !!!
 */
uint16_t set_series(adf_t *, const series_t *, uint32_t);

/*
 * !!! This function doesn't compare the fields `code_idx` !!!
 */
bool are_additive_t_equal(additive_t, additive_t, float);

/*
 * !!! This function doesn't compare the fields `repeated` !!!
 */
bool are_series_equal(const series_t *, const series_t *, const adf_t*);

/*
 * Helper functions to create adf structures.
 */
additive_t create_additive(uint32_t code, float concentration);
wavelength_info_t create_wavelength_info(uint16_t min_w_len_nm,
										 uint16_t max_w_len_nm,
										 uint16_t n_wavelength);
soil_depth_info_t create_soil_depth_info(uint16_t max_soil_depth_mm,
										 uint16_t n_depth);
soil_depth_info_t create_trans_soil_depth_info(uint16_t max_soil_depth_mm,
											   uint16_t n_depth, uint16_t t_y);
reduction_info_t default_reduction_info(void);
reduction_info_t create_reduction_info(uint8_t soil_density_red_mode,
									   uint8_t pressure_red_mode,
									   uint8_t light_exposure_red_mode,
									   uint8_t water_use_red_mode,
									   uint8_t soil_temp_red_mode,
									   uint8_t env_temp_red_mode);
precision_info_t default_precision_info(void);
precision_info_t create_precision_info(float soil_density_prec,
									   float pressure_prec,
									   float light_exposure_prec,
									   float water_use_prec,
									   float soil_temp_prec,
									   float env_temp_prec,
									   float additive_prec);
adf_header_t create_header(uint8_t farming_tec, wavelength_info_t wave_info,
						   soil_depth_info_t soil_info,
						   reduction_info_t reduction_info,
						   precision_info_t precision_info, uint32_t n_chunks);
series_t create_series(float *light_exposure, float *soil_temp_c,
					   float *env_temp_c, float *water_use_ml, uint8_t pH,
					   float p_bar, float soil_density_kg_m3,
					   uint16_t n_soil_adds, uint16_t n_atm_adds,
					   additive_t *soil_additives, additive_t *atm_additives,
					   uint32_t repeated);

/*
 * Helper functions to create dynamically allocated adf structures. They are
 * mainly used in the APIs for those languages who don't have explicit pointers
 * syntax (eg. Java or Javascript).
 */
additive_t *new_additive(uint32_t code, float concentration);
wavelength_info_t *new_wavelength_info(uint16_t min_w_len_nm,
									   uint16_t max_w_len_nm,
									   uint16_t n_wavelength);
soil_depth_info_t *new_soil_depth_info(uint16_t max_soil_depth_mm,
									   uint16_t n_depth);
soil_depth_info_t *new_trans_soil_depth_info(uint16_t max_soil_depth_mm,
											 uint16_t n_depth, uint16_t t_y);
reduction_info_t *new_default_reduction_info(void);
reduction_info_t *new_reduction_info(uint8_t soil_density_red_mode,
									 uint8_t pressure_red_mode,
									 uint8_t light_exposure_red_mode,
									 uint8_t water_use_red_mode,
									 uint8_t soil_temp_red_mode,
									 uint8_t env_temp_red_mode);
precision_info_t *new_default_precision_info(void);
precision_info_t *new_precision_info(float soil_density_prec,
									 float pressure_prec,
									 float light_exposure_prec,
									 float water_use_prec,
									 float soil_temp_prec,
									 float env_temp_prec,
									 float additive_prec);
adf_header_t *new_header(uint8_t farming_tec, wavelength_info_t wave_info,
						 soil_depth_info_t soil_info,
						 reduction_info_t reduction_info,
						 precision_info_t precision_info, uint32_t n_chunks);
series_t *new_series(float *light_exposure, float *soil_temp_c,
					 float *env_temp_c, float *water_use_ml, uint8_t pH,
					 float p_bar, float soil_density_kg_m3,
					 uint16_t n_soil_adds, uint16_t n_atm_adds,
					 additive_t *soil_additives, additive_t *atm_additives,
					 uint32_t repeated);
adf_t *new_adf( adf_header_t *, uint32_t);

/*
 * Init functions. Each of them expects the pointer to init to e allocated.
 */
void metadata_init(adf_meta_t *, uint32_t);
void adf_init(adf_t *, adf_header_t, uint32_t);
uint16_t init_empty_series(series_t *series, uint32_t n_chunks,
						   uint16_t n_wavelenght, uint16_t n_depth,
						   uint16_t n_soil_additives, uint16_t n_atm_additives);

/*
 * !!! Those functions does not free the parameter pointers !!!
 * They just free the *content* of the structure. If the structure itself has
 * been dynamically allocated, use the delete functions below instead.
 * All the internarls pointers will be set to NULL.
 */
void metadata_free(adf_meta_t *);
void series_free(series_t *);
void adf_free(adf_t *);

/*
 * !!! Those functions free the paramenter pointers too !!!
 */
void metadata_delete(adf_meta_t *);
void series_delete(series_t *);
void adf_delete(adf_t *);

/*
 * Each of those functions expect the target pointer to be allocated. They
 * perform a deep copy of the *values* of each structures.
 */
uint16_t cpy_additive(additive_t *, const additive_t *);
uint16_t cpy_adf(adf_t *, const adf_t *);
uint16_t cpy_adf_header(adf_header_t *, const adf_header_t *);
uint16_t cpy_adf_metadata(adf_meta_t *, const adf_meta_t *);
uint16_t cpy_adf_series(series_t *, const series_t *, const adf_t *);

/* */
uint16_t cpy_series_starting_at(series_t *, const adf_t *, uint32_t);

#ifdef __cplusplus
}
#endif

#endif /* __ADF_H__ */
