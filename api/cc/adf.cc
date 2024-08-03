/* 
 * adf.cc - Implementation of the C++ interface for the ADF library
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

#include "adf.hpp"
#include "adf.h"
#include "exceptions.hpp"
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <cstdint>
#include <string> 
#include <vector>

namespace adf {

Adf::Adf(Header header, uint32_t periodSec)
{
	adf_init(&this->adf, header.toCHeader(), periodSec);
}

Adf::Adf(std::vector<std::byte> bytes)
{
	std::vector<uint8_t> cBytes;
	std::function<uint8_t(std::byte)> toCBytes = [] (std::byte x) {return std::to_integer<uint8_t>(x);};
	cBytes = map(bytes, toCBytes);
	uint16_t res = ::unmarshal(&this->adf, cBytes.data());
	if (res != ADF_OK) throwAdfError(res);
}

Adf::~Adf()
{
	adf_free(&this->adf);
}

Version Adf::version(void)
{
	return {
		.major = (__ADF_VERSION__ & MAJOR_VERSION_MASK) >> 8,
		.minor = (__ADF_VERSION__ & MINOR_VERSION_MASK) >> 4,
		.patch = __ADF_VERSION__ & PATCH_VERSION_MASK,
	};
}

std::string Adf::versionString(void)
{
	Version v = this->version();
	#if __cplusplus >= 202002L
	return std::format("{}.{}.{}", v.major, v.minor, v.patch);
	#else
	return std::to_string(v.major) + "." + std::to_string(v.minor) + "." + std::to_string(v.patch);
	#endif
}

void Adf::addSeries(Series& series)
{
	series_t cSeries = series.toCSeries();
	uint16_t res = add_series(&this->adf, &cSeries);
	if (res != ADF_OK) throwAdfError(res);
}

void Adf::updateSeries(Series& series, uint64_t time)
{
	series_t cSeries = series.toCSeries();
	uint16_t res = update_series(&this->adf, &cSeries, time);
	if (res != ADF_OK) throwAdfError(res);
}

void Adf::removeSeries(void)
{
	uint16_t res = remove_series(&this->adf);
	if (res != ADF_OK) throwAdfError(res);
}

std::vector<std::byte> Adf::marshal(void)
{
	std::vector<std::byte> result;
	size_t adf_size = size_adf_t(&this->adf);
	uint8_t *bytes = (uint8_t *) malloc(adf_size * sizeof(uint8_t));
	uint16_t res = ::marshal(bytes, &this->adf);
	if (res != ADF_OK) {
		free(bytes);
		throwAdfError(res);
	}
	for (uint32_t i = 0; i < adf_size; i++) {
		result.push_back(std::byte{bytes[i]});
	}
	free(bytes);
	return result;
}

size_t Adf::size(void) { return size_adf_t(&this->adf); }

} /* namespace adf */
