/* errors.go - AdfError struct and error messages
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

package adflib

// #cgo CFLAGS: -std=c2x
// #include <adf.h>
import "C"
import (
	"errors"
	"fmt"
)

const (
	HEADER_CORRUPTED_STR     string = C.ADF_ERROR_PREFIX + C.ADF_HEADER_CORRUPTED_STR
	METADATA_CORRUPTED_STR   string = C.ADF_ERROR_PREFIX + C.ADF_METADATA_CORRUPTED_STR
	SERIES_CORRUPTED_STR     string = C.ADF_ERROR_PREFIX + C.ADF_SERIES_CORRUPTED_STR
	ZERO_REPEATED_SERIES_STR string = C.ADF_ERROR_PREFIX + C.ADF_ZERO_REPEATED_SERIES_STR
	EMPTY_SERIES_STR         string = C.ADF_ERROR_PREFIX + C.ADF_EMPTY_SERIES_STR
	TIME_OUT_OF_BOUND_STR    string = C.ADF_ERROR_PREFIX + C.ADF_TIME_OUT_OF_BOUND_STR
	ADDITIVE_OVERFLOW_STR    string = C.ADF_ERROR_PREFIX + C.ADF_ADDITIVE_OVERFLOW_STR
	NULL_HEADER_SOURCE_STR   string = C.ADF_ERROR_PREFIX + C.ADF_NULL_HEADER_SOURCE_STR
	NULL_HEADER_TARGET_STR   string = C.ADF_ERROR_PREFIX + C.ADF_NULL_HEADER_TARGET_STR
	NULL_META_SOURCE_STR     string = C.ADF_ERROR_PREFIX + C.ADF_NULL_META_SOURCE_STR
	NULL_META_TARGET_STR     string = C.ADF_ERROR_PREFIX + C.ADF_NULL_META_TARGET_STR
	NULL_SERIES_SOURCE_STR   string = C.ADF_ERROR_PREFIX + C.ADF_NULL_SERIES_SOURCE_STR
	NULL_SERIES_TARGET_STR   string = C.ADF_ERROR_PREFIX + C.ADF_NULL_SERIES_TARGET_STR
	NULL_SOURCE_STR          string = C.ADF_ERROR_PREFIX + C.ADF_NULL_SOURCE_STR
	NULL_TARGET_STR          string = C.ADF_ERROR_PREFIX + C.ADF_NULL_TARGET_STR
	NULL_ADDITIVE_SOURCE_STR string = C.ADF_ERROR_PREFIX + C.ADF_NULL_ADDITIVE_SOURCE_STR
	NULL_ADDITIVE_TARGET_STR string = C.ADF_ERROR_PREFIX + C.ADF_NULL_ADDITIVE_TARGET_STR
	RUNTIME_ERROR_STR        string = C.ADF_ERROR_PREFIX + C.ADF_RUNTIME_ERROR_STR
)

type AdfError struct {
	Code uint16
	Err  error
}

func (r *AdfError) Error() string {
	return fmt.Sprintf("ERROR %d: %s", r.Code, r.Err.Error())
}

func getAdfError(errorCode uint16) *AdfError {
	adfError := AdfError{Code: errorCode}
	switch ErrorCode(errorCode) {
	case HEADER_CORRUPTED:
		adfError.Err = errors.New(HEADER_CORRUPTED_STR)
	case METADATA_CORRUPTED:
		adfError.Err = errors.New(METADATA_CORRUPTED_STR)
	case SERIES_CORRUPTED:
		adfError.Err = errors.New(SERIES_CORRUPTED_STR)
	case ZERO_REPEATED_SERIES:
		adfError.Err = errors.New(ZERO_REPEATED_SERIES_STR)
	case EMPTY_SERIES:
		adfError.Err = errors.New(EMPTY_SERIES_STR)
	case TIME_OUT_OF_BOUND:
		adfError.Err = errors.New(TIME_OUT_OF_BOUND_STR)
	case ADDITIVE_OVERFLOW:
		adfError.Err = errors.New(ADDITIVE_OVERFLOW_STR)
	case NULL_HEADER_SOURCE:
		adfError.Err = errors.New(NULL_HEADER_SOURCE_STR)
	case NULL_HEADER_TARGET:
		adfError.Err = errors.New(NULL_HEADER_TARGET_STR)
	case NULL_META_SOURCE:
		adfError.Err = errors.New(NULL_META_SOURCE_STR)
	case NULL_META_TARGET:
		adfError.Err = errors.New(NULL_META_TARGET_STR)
	case NULL_SERIES_SOURCE:
		adfError.Err = errors.New(NULL_SERIES_SOURCE_STR)
	case NULL_SERIES_TARGET:
		adfError.Err = errors.New(NULL_SERIES_TARGET_STR)
	case NULL_SOURCE:
		adfError.Err = errors.New(NULL_SOURCE_STR)
	case NULL_TARGET:
		adfError.Err = errors.New(NULL_TARGET_STR)
	case NULL_ADDITIVE_SOURCE:
		adfError.Err = errors.New(NULL_ADDITIVE_SOURCE_STR)
	case NULL_ADDITIVE_TARGET:
		adfError.Err = errors.New(NULL_ADDITIVE_TARGET_STR)
	case RUNTIME_ERROR:
		adfError.Err = errors.New(RUNTIME_ERROR_STR)
	default:
		return nil
	}
	return &adfError
}
