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
#include "exceptions.hpp"
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <stdint.h>
#include <vector>

namespace adf {

wavelength_info_t WaveInfo::toCWaveInfo(void)
{
	wavelength_info_t w_info = {
		.max_w_len_nm = { this->getMaxWlenNm() },
		.min_w_len_nm = { this->getMinWlenNm() },
		.n_wavelength = { this->getNWavelength() },
	};
	return w_info;
}

soil_depth_info_t SoilDepthInfo::toCSoilInfo(void)
{
	soil_depth_info_t s_info = {
		.n_depth = { this->getNDepthMeasurements() },
		.min_soil_depth_mm = { this->getMinDepthMm() },
		.max_soil_depth_mm = { this->getMaxDepthMm() }
	};
	return s_info;
}

reduction_info_t ReductionInfo::toCReductionInfo(void)
{
	reduction_info_t r_info = {
		.soil_density_red_mode = soilDensity,
		.pressure_red_mode = pressure,
		.light_exposure_red_mode = lightExposure,
		.water_use_red_mode = waterUse,
		.soil_temp_red_mode = soilTemp,
		.env_temp_red_mode = envTemp
	};
	return r_info;
}

adf_header_t AdfHeader::toCHeader(void)
{
	return create_header(
		this->getFarmingTec(),
		this->waveInfo.toCWaveInfo(),
		this->soilDepthInfo.toCSoilInfo(),
		this->reductionInfo.toCReductionInfo(),
		this->getNChunks()
	);
}

additive_t AdfAdditive::toCAdditive(void)
{
	additive_t add =  {
		.code = { this->code },
		.concentration = { this->concentration }
	};
	return add;
}

series_t AdfSeries::toCSeries(void)
{
	std::function<additive_t(AdfAdditive)> f = [](AdfAdditive add) { return add.toCAdditive(); };
	
	real_t lightExposureFirstElem = { *this->getLightexposure().startPointer() };
	real_t soilTempFirstElem = { *this->getSoilTemperatureCelsius().startPointer() };
	real_t envTempFirstElem =  { *this->getEnvironmenttemperatureCelsius().data() };
	real_t waterUseFirstElem = { *this->getWateruseml().data() };
	additive_t soilAdditives =  { *map(this->getSoiladditives(), f).data() };
	additive_t atmosphereAdditives =  { *map(this->getAtmosphereadditives(), f).data() };
	series_t cSeries = {
		.light_exposure = &lightExposureFirstElem,
		.soil_temp_c = &soilTempFirstElem,
		.env_temp_c = &envTempFirstElem,
		.water_use_ml = &waterUseFirstElem,
		.pH = (uint8_t)(this->getPh() * 10),
		.p_bar = { this->getPressurebar() },
		.soil_density_kg_m3 = { this->getSoildensitykgm3() },
		.n_soil_adds= { (uint16_t)this->getSoiladditives().size() },
		.n_atm_adds = {(uint16_t) this->getAtmosphereadditives().size() },
		.soil_additives = &soilAdditives,
		.atm_additives = &atmosphereAdditives,
		.repeated = {this->getRepeated() }
	};

	return cSeries;
}

std::string Adf::version(void)
{
	uint8_t major = (__ADF_VERSION__ & MAJOR_VERSION_MASK) >> 8;
	uint8_t minor = (__ADF_VERSION__ & MINOR_VERSION_MASK) >> 4;
	uint8_t patch = __ADF_VERSION__ & PATCH_VERSION_MASK;
	return std::format("{}.{}.{}", major, minor, patch);
}

void Adf::addSeries(AdfSeries &series)
{
	series_t cSeries = series.toCSeries();
	uint16_t res = add_series(&this->adf, &cSeries);
	if (res != ADF_OK) throwAdfError(res);
}

void Adf::updateSeries(AdfSeries &series, uint64_t time)
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

std::vector<std::byte> Adf::marshal()
{
	std::vector<std::byte> result;
	std::vector<uint8_t> bytesVec;
	std::function<std::byte(uint8_t)> toCppBytes = [] (uint8_t x) {return std::byte{x};};
	size_t adf_size = size_adf_t(&this->adf);
	uint8_t *bytes = (uint8_t *) malloc(adf_size * sizeof(uint8_t));
	uint16_t res = ::marshal(bytes, &this->adf);
	if (res != ADF_OK) {
		free(bytes);
		throwAdfError(res);
	}
	bytesVec.assign(bytes, bytes + adf_size);
	result = map(bytesVec, toCppBytes);
	free(bytes);
	return result;
}

size_t Adf::size(void) { return size_adf_t(&this->adf); }

Adf::Adf(std::vector<std::byte> bytes)
{
	std::vector<uint8_t> cBytes;
	std::function<uint8_t(std::byte)> toCBytes = [] (std::byte x) {return std::to_integer<uint8_t>(x);};
	cBytes = map(bytes, toCBytes);
	uint16_t res = ::unmarshal(&this->adf, cBytes.data());
	if (res != ADF_OK) throwAdfError(res);
}

} /* namespace adf */
