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

#include <adf.h>
#include <cstdint>
#include <format>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace adf {

template<typename T, typename U>
static std::vector<U> map(const std::vector<T> &vec, std::function<U(T)> f)
{
	std::vector<U> newVec;
	for (auto const &i : vec) {
		newVec.push_back(f(i));
	}
	return newVec;
}

class InvalidMatrixShapeException : public std::exception {
	private:
  	using std::exception::what;
	std::string message;

	public:
	InvalidMatrixShapeException(uint32_t size, uint32_t rows, uint32_t columns)
	{
		#if __cplusplus >= 202002L
		this->message = std::format(
			"An array of size {}, cannot be expressed as a matrix of shape {}x{}", 
			size,
			rows,
			columns
		);
		#else
		this->message = "An array of size " + std::to_string(size) +
						", cannot be expressed as a matrix of shape " 
						+ std::to_string(rows) + "x" + std::to_string(columns);
		#endif
	}
	
	const char *what() { return (const char *)message.c_str(); }
};

template <typename T> 
class Matrix {
	private:
	uint32_t nRows;
	uint32_t nColumns;
	std::vector<T> mat;

	public:
	Matrix(uint32_t rows, uint32_t columns)
	{
		this->mat.resize(rows * columns, 0);
		this->nRows = rows;
		this->nColumns = columns;
	}
	Matrix(std::vector<T> init, uint32_t rows, uint32_t columns)
	{
		if (init.size() != rows * columns)
			throw InvalidMatrixShapeException(init.size(), rows, columns);
		this->mat = init;
		this->nRows = rows;
		this->nColumns = columns;
	}

	std::vector<T> getVector() { return this->mat; }
	void addRow(std::vector<T> &row)
	{
		if (row.size() != this->nColumns)
			throw InvalidMatrixShapeException(row.size(), this->nRows, this->nColumns);
		this->mat.insert(this->mat.end(), row.begin(), row.end());
	}
	T *startPointer() { return this->mat.data(); }
	T &operator()(uint32_t row, uint32_t column) 
	{
		return this->mat.at(column + row * nColumns);
	}
};

class WaveInfo {
	private:
	uint16_t nWavelength;
	uint16_t minWlenNm;
	uint16_t maxWlenNm;
	wavelength_info_t toCWaveInfo(void);

	public:
	WaveInfo(uint16_t nWavelength, uint16_t minWlenNm, uint16_t maxWlenNm)
		: nWavelength(nWavelength), minWlenNm(minWlenNm), maxWlenNm(maxWlenNm)
	{ }
	uint16_t getNWavelength(void) { return this->nWavelength; }
	uint16_t getMinWlenNm(void) { return this->minWlenNm; }
	uint16_t getMaxWlenNm(void) { return this->maxWlenNm; }
	friend class Header;
};

class SoilDepthInfo {
	private:
	uint16_t nDepthMeasurements;
	uint16_t minDepthMm;
	uint16_t maxDepthMm;
	soil_depth_info_t toCSoilInfo(void);

	public:
	SoilDepthInfo(uint16_t nDepthMeasurements, uint16_t minDepthMm,
				  uint16_t maxDepthMm)
		: nDepthMeasurements(nDepthMeasurements), minDepthMm(minDepthMm),
		  maxDepthMm(maxDepthMm)
	{ }
	uint16_t getNDepthMeasurements(void) { return this->nDepthMeasurements; }
	uint16_t getMinDepthMm(void) { return this->minDepthMm; }
	uint16_t getMaxDepthMm(void) { return this->maxDepthMm; }
	friend class Header;
};

class ReductionInfo {
	private:
	uint8_t soilDensity;
	uint8_t pressure;
	uint8_t lightExposure;
	uint8_t waterUse;
	uint8_t soilTemp;
	uint8_t envTemp;
	reduction_info_t toCReductionInfo(void);

	public:
	/* Init every field to 0. */
	ReductionInfo() { }
	ReductionInfo(uint8_t soilDensity, uint8_t pressure, uint8_t lightExposure,
				  uint8_t waterUse, uint8_t soilTemp, uint8_t envTemp)
		: soilDensity(soilDensity), pressure(pressure),
		  lightExposure(lightExposure), waterUse(waterUse),
		  soilTemp(soilTemp), envTemp(envTemp) 
	{ }
	uint8_t getSoilDensity(void) { return this->soilDensity; }
	uint8_t getPressure(void) { return this->pressure; }
	uint8_t getLightExposure(void) { return this->lightExposure; }
	uint8_t getWaterUse(void) { return this->waterUse; }
	uint8_t getSoilTemp(void) { return this->soilTemp; }
	uint8_t getEnvTemp(void) { return this->envTemp; }
	friend class Header;
};

class Header {
  private:
	uint8_t farmingTec;
	WaveInfo waveInfo;
	SoilDepthInfo soilDepthInfo;
	ReductionInfo reductionInfo;
	uint32_t nChunks;
	adf_header_t toCHeader(void);

  public:
	Header(uint8_t farmingTec, WaveInfo waveInfo, SoilDepthInfo depthInfo,
		   ReductionInfo reductionInfo, uint32_t nChunks)
		: farmingTec(farmingTec), waveInfo(waveInfo), soilDepthInfo(depthInfo),
		  reductionInfo(reductionInfo), nChunks(nChunks)
	{ }

	uint8_t getFarmingTec(void) { return this->farmingTec; }
	uint32_t getNChunks(void) { return this->nChunks; }
	WaveInfo getWaveInfo(void) { return this->waveInfo; }
	SoilDepthInfo getDepthInfo(void) { return this->soilDepthInfo; }

	friend class Adf;
};

class Additive {
  private:
	uint16_t codeIdx;
	uint32_t code;
	float concentration;
	additive_t toCAdditive(void);

  public:
	Additive(uint32_t code, float concentration)
		: code(code), concentration(concentration)
	{ }
	Additive(uint16_t codeIdx, uint32_t code, float concentration)
		: codeIdx(codeIdx), code(code), concentration(concentration)
	{ }

	uint16_t getCodeIdx(void) { return this->codeIdx; }
	uint32_t getCode(void) { return this->code; }
	float getConcentration(void) { return this->concentration; }

	friend class Series;
};

class Series {
	private:
	Matrix<float> lightExposure;
	Matrix<float> soilTempCelsius;
	std::vector<float> environmentTempCelsius;
	std::vector<float> wateruseMl;
	float pH;
	float pressureBar;
	float soilDensityKgM3;
	std::vector<Additive> soilAdditives;
	std::vector<Additive> atmosphereAdditives;
	uint32_t repeated;
	series_t toCSeries(void);

	public:
	Series(Matrix<float> lightExposure,
			  Matrix<float> soilTemperatureCelsius,
			  std::vector<float> environmenttemperatureCelsius,
			  std::vector<float> wateruseMl, float pH, float pressureBar,
			  float soilDensityKgM3, std::vector<Additive> soilAdditives,
			  std::vector<Additive> atmosphereAdditives, uint32_t repeated)
		: lightExposure(lightExposure), soilTempCelsius(soilTemperatureCelsius),
		  environmentTempCelsius(environmenttemperatureCelsius),
		  wateruseMl(wateruseMl), pH(pH), pressureBar(pressureBar),
		  soilDensityKgM3(soilDensityKgM3), soilAdditives(soilAdditives),
		  atmosphereAdditives(atmosphereAdditives), repeated(repeated)
	{ }

	Matrix<float> getLightexposure(void) { return this->lightExposure; }
	Matrix<float> getSoilTemperatureCelsius(void)
	{
		return this->soilTempCelsius; 
	}
	std::vector<float> getEnvironmenttemperatureCelsius(void)
	{
		return this->environmentTempCelsius;
	}
	std::vector<float> getWateruseml(void) { return this->wateruseMl; }
	float getPh(void) { return this->pH; }
	float getPressurebar(void) { return this->pressureBar; }
	float getSoildensitykgm3(void) { return this->soilDensityKgM3; }
	std::vector<Additive> getSoiladditives(void)
	{
		return this->soilAdditives;
	}
	std::vector<Additive> getAtmosphereadditives(void)
	{
		return this->atmosphereAdditives;
	}
	uint32_t getRepeated(void) { return this->repeated; }

	friend class Adf;
};

struct Version {
	uint8_t major;
	uint8_t minor;
	uint8_t patch;
};

class Adf {
	private:
	adf_t adf;

	public:
	Adf(Header header, uint32_t periodSec)
	{
		adf_init(&this->adf, header.toCHeader(), periodSec);
	}
	/* Unmarshal and init */
	Adf(std::vector<std::byte> bytes);
	~Adf() { adf_free(&this->adf); }
	std::string versionString(void);
	Version version(void);
	size_t size(void);
	void addSeries(Series &series);
	void removeSeries(void);
	void updateSeries(Series &series, uint64_t time);
	std::vector<std::byte> marshal();
};
} /* namespace adf */
