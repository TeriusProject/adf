/* adf.hpp
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

#include "adf.h"
#include <cstdint>
#include <vector>
#include <iostream>

namespace adf {

const uint32_t ADF_DAY = 86'400;
const uint32_t ADF_WEEK = 604'800;
const uint32_t ADF_MONTH_28 = 2'419'200;
const uint32_t ADF_MONTH_29 = 2'505'600;
const uint32_t ADF_MONTH_30 = 2'592'000;
const uint32_t ADF_MONTH_31 = 2'678'400;

class AdfWaveInfo {
private:
	uint32_t nWavelength;
	uint32_t minWlenNm;
	uint32_t maxWlenNm;
	
public:
	AdfWaveInfo(uint32_t nWavelength, uint32_t minWlenNm, uint32_t maxWlenNm) :
	nWavelength(nWavelength),
	minWlenNm(minWlenNm),
	maxWlenNm(maxWlenNm) { }
	
	uint32_t getNWavelength() { return this->nWavelength; }
	uint32_t getMinWlenNm() { return this->minWlenNm; }
	uint32_t getMaxWlenNm() { return this->maxWlenNm; }
};

class AdfHeader {
  private:
	const uint32_t signature = __ADF_SIGNATURE__;
	const uint8_t version = __ADF_VERSION__;
	uint8_t farmingTec;
	uint32_t nChunks;
	AdfWaveInfo waveInfo;
	
public:
	AdfHeader(uint8_t farmingTec, uint32_t nChunks, AdfWaveInfo waveInfo) :
	farmingTec(farmingTec),
	nChunks(nChunks),
	waveInfo(waveInfo) { }
	
	uint32_t getSignature() { return this->signature; }
	uint8_t getVersion() { return this->version; }
	uint8_t getFarmingTec() { return this->farmingTec; }
	uint32_t getNChunks() { return this->nChunks; }
	AdfWaveInfo getWaveInfo() { return this->waveInfo; }
};

class AdfMetadata {
	
};

class AdfAdditive {
private:
	uint16_t codeIdx;
	uint32_t code;
	float concentration;
public:
	AdfAdditive(uint32_t code, float concentration) :
	concentration(concentration),
	code(code) { }
	
	AdfAdditive(uint16_t codeIdx, uint32_t code, float concentration) :
	concentration(concentration),
	code(code),
	codeIdx(codeIdx) { }
	
	uint16_t getCodeIdx() { return this->codeIdx; }
	uint32_t getCode() { return this->code; }
	float getConcentration() { return this->concentration; }
};

class AdfSeries {
private:
	std::vector<float> lightExposure;
	std::vector<float> temperatureCelsius;
	std::vector<float> wateruseMl;
	float pH;
	float pressureBar;
	float soilDensityKgM3;
	std::vector<AdfAdditive> soilAdditives;
	std::vector<AdfAdditive> atmosphereAdditives;
	uint32_t repeated;
public:
	AdfSeries(std::vector<float> lightExposure, 
			  std::vector<float> temperatureCelsius,
			  std::vector<float> wateruseMl,
			  float pH,
			  float pressureBar,
			  float soilDensityKgM3,
			  std::vector<AdfAdditive> soilAdditives,
			  std::vector<AdfAdditive> atmosphereAdditives,
			  uint32_t repeated
) :
	lightExposure(lightExposure),
	temperatureCelsius(temperatureCelsius),
	wateruseMl(wateruseMl),
	pH(pH),
	pressureBar(pressureBar),
	soilDensityKgM3(soilDensityKgM3),
	soilAdditives(soilAdditives),
	atmosphereAdditives(atmosphereAdditives),
	repeated(repeated) { }
	
	std::vector<float> getLightexposure() {
	return this->lightExposure;
	}
	
	std::vector<float> getTemperaturecelsius() {
	return this->temperatureCelsius;
	}
	
	std::vector<float> getWateruseml() {
	return this->wateruseMl;
	}
	
	float getPh() {
	return this->pH;
	}
	
	float getPressurebar() {
	return this->pressureBar;
	}
	
	float getSoildensitykgm3() {
	return this->soilDensityKgM3;
	}
	
	std::vector<AdfAdditive> getSoiladditives() {
	return this->soilAdditives;
	}
	
	std::vector<AdfAdditive> getAtmosphereadditives() {
	return this->atmosphereAdditives;
	}
	
	uint32_t getRepeated() {
	return this->repeated;
	}
};

class Adf {
  private:
	adf_t adf;

  public:
	Adf(AdfHeader header, uint16_t periodSec) {
		adf_header_t _header = (adf_header_t) {
			.signature = { header.getSignature() },
			.version = header.getVersion(),
			.farming_tec = header.getFarmingTec(),
			.max_w_len_nm = { header.getWaveInfo().getMaxWlenNm() },
			.min_w_len_nm = { header.getWaveInfo().getMinWlenNm() },
			.n_wavelength = { header.getWaveInfo().getNWavelength() },
			.n_chunks = { header.getNChunks() }
		};
		adf_meta_t metadata = (adf_meta_t) {
			.period_sec = {periodSec},
			.n_series = 0,
			.size_series  ={0},
			.n_additives = {0},
			.additive_codes = NULL
		};
		this->adf = create_adf(_header, metadata);
	}

	~Adf() noexcept { adf_free(&this->adf); }
	void addSeries(AdfSeries &series);
	void removeSeries(void);
	
	std::vector<std::byte> marshal();
	static Adf unmarshal(std::vector<std::byte> bytes);
};

} // namespace adf

//std::ostream& operator<<(std::ostream& os, adf::Adf adf)
//{
//	return os << adf.marshal();
//}
