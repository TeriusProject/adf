/* 
 * exceptions.hpp - wrappers for ADF error codes
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

#pragma once

#include <adf.h>
#include <string>
#include <format> 

namespace adf {

void throwAdfError(uint32_t errorCode)
{
	switch (errorCode) {
		case ADF_HEADER_CORRUPTED:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_HEADER_CORRUPTED_STR);
		case ADF_METADATA_CORRUPTED:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_METADATA_CORRUPTED_STR);
		case ADF_SERIES_CORRUPTED:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_SERIES_CORRUPTED_STR);
		case ADF_ZERO_REPEATED_SERIES:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_ZERO_REPEATED_SERIES_STR);
		case ADF_EMPTY_SERIES:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_EMPTY_SERIES_STR);
		case ADF_TIME_OUT_OF_BOUND:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_TIME_OUT_OF_BOUND_STR);
		case ADF_ADDITIVE_OVERFLOW:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_ADDITIVE_OVERFLOW_STR);
		case ADF_NULL_HEADER_SOURCE:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_HEADER_SOURCE_STR);
		case ADF_NULL_HEADER_TARGET:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_HEADER_TARGET_STR);
		case ADF_NULL_META_SOURCE:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_META_SOURCE_STR);
		case ADF_NULL_META_TARGET:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_META_TARGET_STR);
		case ADF_NULL_SERIES_SOURCE:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_SERIES_SOURCE_STR);
		case ADF_NULL_SERIES_TARGET:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_SERIES_TARGET_STR);
		case ADF_NULL_SOURCE:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_SOURCE_STR);
		case ADF_NULL_TARGET:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_TARGET_STR);
		case ADF_NULL_ADDITIVE_SOURCE:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_ADDITIVE_SOURCE_STR);
		case ADF_NULL_ADDITIVE_TARGET:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_NULL_ADDITIVE_TARGET_STR);
		case ADF_RUNTIME_ERROR:
			throw std::runtime_error(ADF_ERROR_PREFIX ADF_RUNTIME_ERROR_STR);
		default:
			break;
	}
}
} /* namespace adf */
