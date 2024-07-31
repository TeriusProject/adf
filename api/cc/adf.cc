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
#include <vector>
#include <functional>
#include <format>
#include <fstream>
#include <iostream>

namespace adf {

adf_header_t AdfHeader::toCHeader(void)
{
	adf_header_t header = {
			.signature = { this->getSignature() },
			.version = { this->getVersion() },
			.farming_tec = this->getFarmingTec(),
			.max_w_len_nm = { this->getWaveInfo().getMaxWlenNm() },
			.min_w_len_nm = { this->getWaveInfo().getMinWlenNm() },
			.n_wavelength = { this->getWaveInfo().getNWavelength() },
			.n_chunks = { this->getNChunks() },
			.n_depth = { this->getDepthInfo().getNDepthMeasurements() },
			.min_soil_depth_mm = { this->getDepthInfo().getMinDepthMm() },
			.max_soil_depth_mm = { this->getDepthInfo().getMaxDepthMm() }
		};
	return header;
}

series_t AdfSeries::toCSeries(void)
{
	std::function<additive_t(AdfAdditive)> f = [](AdfAdditive add) {
		additive_t tmp =  {
			.code = {add.getCode()},
			.concentration = {add.getConcentration()}
		};
		return tmp;
	};
	
	real_t lightExposureFirstElem = { *this->getLightexposure().getFirstElement() };
	real_t soilTempFirstElem = { *this->getSoilTemperatureCelsius().getFirstElement() };
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

std::string Adf::getVersion(void)
{
	uint8_t major = (__ADF_VERSION__ & MAJOR_VERSION_MASK) >> 8;
	uint8_t minor = __ADF_VERSION__ & MINOR_VERSION_MASK;
	return std::format("{}.{}", major, minor);
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
	size_t adf_size = size_adf_t(this->adf);
	uint8_t *bytes = (uint8_t *) malloc(adf_size * sizeof(uint8_t));
	uint16_t res = ::marshal(bytes, this->adf);
	if (res != ADF_OK) {
		free(bytes);
		throwAdfError(res);
	}
	bytesVec.assign(bytes, bytes + adf_size);
	result = map(bytesVec, toCppBytes);
	free(bytes);
	return result;
}

Adf::Adf(std::vector<std::byte> bytes)
{
	std::vector<uint8_t> cBytes;
	std::function<uint8_t(std::byte)> toCBytes = [] (std::byte x) {return std::to_integer<uint8_t>(x);};
	cBytes = map(bytes, toCBytes);
	uint16_t res = ::unmarshal(&this->adf, cBytes.data());
	if (res != ADF_OK) throwAdfError(res);
}

} // namespace adf

int main() 
{
	adf::AdfHeader header = adf::AdfHeader(1, 2, adf::WaveInfo(2, 350, 1000),
	adf::SoilDepthInfo(0,0,0));
	adf::Adf a = adf::Adf(header, 3600);
	adf::Matrix<float> lightExposure({234,343,5,3},2,2);
	adf::Matrix<float> soilTemperature({1,2,3,4}, 2,2);
	std::vector<float> temperatureCelsius({23,3,3,2,5});
	std::vector<float> waterUseMl({1,66,3,33,46});
	std::vector<adf::AdfAdditive> soilAdditives({adf::AdfAdditive(1, 16.504)});
	std::vector<adf::AdfAdditive> atmosphereAdditives({adf::AdfAdditive(2, 3.5)});
	
	adf::AdfSeries series(lightExposure,
					soilTemperature,
				   temperatureCelsius,
				   waterUseMl,
				   2.5,
				   3324.67,
				   11.0,
				   soilAdditives,
				   atmosphereAdditives,
				   1);
	a.addSeries(series);
	std::vector<std::byte> arr = a.marshal();
	arr = a.marshal();
	arr = a.marshal();
	adf::Adf new_adf(arr);
	adf::Adf new_adf2(arr);
	adf::Adf new_adf3(arr);
	std::cout << new_adf2.getVersion() << std::endl;
}
