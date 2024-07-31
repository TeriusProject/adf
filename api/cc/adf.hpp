/* 
 * adf.hpp - C++ interface for the ADF library
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

#include "utils.hpp"
#include <adf.h>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <vector>

namespace adf {

class WaveInfo {
	private:
	uint16_t nWavelength;
	uint16_t minWlenNm;
	uint16_t maxWlenNm;

	public:
	WaveInfo(uint16_t nWavelength, uint16_t minWlenNm, uint16_t maxWlenNm)
		: nWavelength(nWavelength), minWlenNm(minWlenNm), maxWlenNm(maxWlenNm)
	{ }

	uint16_t getNWavelength(void) { return this->nWavelength; }
	uint16_t getMinWlenNm(void) { return this->minWlenNm; }
	uint16_t getMaxWlenNm(void) { return this->maxWlenNm; }
};

class SoilDepthInfo {
	private:
	uint16_t nDepthMeasurements;
	uint16_t minDepthMm;
	uint16_t maxDepthMm;

	public:
	SoilDepthInfo(uint16_t nDepthMeasurements, uint16_t minDepthMm, uint16_t maxDepthMm)
		: nDepthMeasurements(nDepthMeasurements), minDepthMm(minDepthMm), maxDepthMm(maxDepthMm)
	{ }

	uint16_t getNDepthMeasurements(void) { return this->nDepthMeasurements; }
	uint16_t getMinDepthMm(void) { return this->minDepthMm; }
	uint16_t getMaxDepthMm(void) { return this->maxDepthMm; }
};

class AdfHeader {
  private:
	uint8_t farmingTec;
	uint32_t nChunks;
	WaveInfo waveInfo;
	SoilDepthInfo depthInfo;
	adf_header_t toCHeader(void);

  public:
	AdfHeader(uint8_t farmingTec, uint32_t nChunks, WaveInfo waveInfo, SoilDepthInfo depthInfo)
		: farmingTec(farmingTec), nChunks(nChunks), waveInfo(waveInfo), depthInfo(depthInfo)
	{
	}

	uint32_t getSignature(void) { return __ADF_SIGNATURE__; }
	uint16_t getVersion(void) { return __ADF_VERSION__; }
	uint8_t getFarmingTec(void) { return this->farmingTec; }
	uint32_t getNChunks(void) { return this->nChunks; }
	WaveInfo getWaveInfo(void) { return this->waveInfo; }
	SoilDepthInfo getDepthInfo(void) { return this->depthInfo; }

	friend class Adf;
};

class AdfAdditive {
  private:
	uint16_t codeIdx;
	uint32_t code;
	float concentration;

  public:
	AdfAdditive(uint32_t code, float concentration)
		: code(code), concentration(concentration)
	{
	}

	AdfAdditive(uint16_t codeIdx, uint32_t code, float concentration)
		: codeIdx(codeIdx), code(code), concentration(concentration)
	{
	}

	uint16_t getCodeIdx(void) { return this->codeIdx; }
	uint32_t getCode(void) { return this->code; }
	float getConcentration(void) { return this->concentration; }
};

class AdfSeries {
	private:
	Matrix<float> lightExposure;
	Matrix<float> soilTemperatureCelsius;
	std::vector<float> environmenttemperatureCelsius;
	std::vector<float> wateruseMl;
	float pH;
	float pressureBar;
	float soilDensityKgM3;
	std::vector<AdfAdditive> soilAdditives;
	std::vector<AdfAdditive> atmosphereAdditives;
	uint32_t repeated;
	series_t toCSeries(void);

	public:
	AdfSeries(Matrix<float> lightExposure,
			  Matrix<float> soilTemperatureCelsius,
			  std::vector<float> environmenttemperatureCelsius,
			  std::vector<float> wateruseMl, float pH, float pressureBar,
			  float soilDensityKgM3, std::vector<AdfAdditive> soilAdditives,
			  std::vector<AdfAdditive> atmosphereAdditives, uint32_t repeated)
		: lightExposure(lightExposure), soilTemperatureCelsius(soilTemperatureCelsius), 
		  environmenttemperatureCelsius(environmenttemperatureCelsius),
		  wateruseMl(wateruseMl), pH(pH), pressureBar(pressureBar),
		  soilDensityKgM3(soilDensityKgM3), soilAdditives(soilAdditives),
		  atmosphereAdditives(atmosphereAdditives), repeated(repeated)
	{ }

	Matrix<float> getLightexposure(void) { return this->lightExposure; }

	Matrix<float> getSoilTemperatureCelsius(void)
	{
		return this->soilTemperatureCelsius;
	}

	std::vector<float> getEnvironmenttemperatureCelsius(void)
	{
		return this->environmenttemperatureCelsius;
	}

	std::vector<float> getWateruseml(void) { return this->wateruseMl; }

	float getPh(void) { return this->pH; }

	float getPressurebar(void) { return this->pressureBar; }

	float getSoildensitykgm3(void) { return this->soilDensityKgM3; }

	std::vector<AdfAdditive> getSoiladditives(void) { return this->soilAdditives; }

	std::vector<AdfAdditive> getAtmosphereadditives(void)
	{
		return this->atmosphereAdditives;
	}

	uint32_t getRepeated(void) { return this->repeated; }

	friend class Adf;
};

class Adf {
	private:
	adf_t adf;

	public:
	Adf(AdfHeader header, uint32_t periodSec)
	{
		adf_init(&this->adf, header.toCHeader(), periodSec);
	}
	Adf(std::vector<std::byte> bytes);
	~Adf()  { adf_free(&this->adf); }
	std::string getVersion(void);
	void addSeries(AdfSeries &series);
	void removeSeries(void);
	void updateSeries(AdfSeries &series, uint64_t time);
	std::vector<std::byte> marshal();
};

} /* namespace adf */
