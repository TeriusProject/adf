/* adf.hpp - C++ interface for the ADF library
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

	class Version : public version_t {
	private:
		Version(version_t v) : Version(v.major, v.minor, v.patch) { }
	public:
		Version(uint8_t major, uint8_t minor, uint8_t patch);

		friend class Adf;
	};

	enum class ReductionCode {
		None          = ADF_RM_NONE,
		Average       = ADF_RM_AVG,
		MovingAverage = ADF_RM_MAVG,
	};

	enum class FarmingTechnique {
		Regular          = ADF_FT_REGULAR,
		Indoor           = ADF_FT_INDOOR,
		IndoorProtected  = ADF_FT_INDOOR_PROTECTED,
		Outdoor          = ADF_FT_OUTDOOR,
		ArtificialSoil   = ADF_FT_ARTIFICIAL_SOIL,
		Hydroponics      = ADF_FT_HYDROPONICS,
		Anthroponics     = ADF_FT_ANTHROPONICS,
		Aeroponics       = ADF_FT_AEROPONICS,
		Fogponics        = ADF_FT_FOGPONICS,
	};

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
				this->message = "An array of size "
								+ std::to_string(size) 
								+ ", cannot be expressed as a matrix of shape " 
								+ std::to_string(rows)
								+ "x"
								+ std::to_string(columns);
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
		Matrix(uint32_t columns)
			{
				this->nRows = 0;
				this->nColumns = columns;
			}
		Matrix(const std::vector<T>& init, uint32_t rows, uint32_t columns)
			{
				if (init.size() != rows * columns)
					throw InvalidMatrixShapeException(init.size(), rows, columns);
				this->mat = init;
				this->nRows = rows;
				this->nColumns = columns;
			}

		std::vector<T> getVector(void) { return this->mat; }
		void addRow(const std::vector<T>& row)
			{
				if (row.size() != this->nColumns)
					throw InvalidMatrixShapeException(row.size(), this->nRows, this->nColumns);
				this->mat.insert(this->mat.end(), row.begin(), row.end());
				this->nRows++;
			}
		T *startPointer(void) { return this->mat.data(); }
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
		WaveInfo(uint16_t nWavelength, uint16_t minWlenNm, uint16_t maxWlenNm);
		uint16_t getNWavelength(void);
		uint16_t getMinWlenNm(void);
		uint16_t getMaxWlenNm(void);

		friend class Header;
	};

	class SoilDepthInfo {
	private:
		uint16_t nDepthMeasurements;
		uint16_t tY;
		uint16_t maxDepthMm;
		soil_depth_info_t toCSoilInfo(void);

	public:
		SoilDepthInfo(uint16_t nDepthMeasurements, uint16_t tY,
					  uint16_t maxDepthMm);
		uint16_t getNDepthMeasurements(void);
		uint16_t getTY(void);
		uint16_t getMaxDepthMm(void);

		friend class Header;
	};


	class ReductionInfo {
	private:
		ReductionCode soilDensity;
		ReductionCode pressure;
		ReductionCode lightExposure;
		ReductionCode waterUse;
		ReductionCode soilTemp;
		ReductionCode envTemp;
		ReductionCode additive;
		reduction_info_t toCReductionInfo(void);

	public:
		ReductionInfo();
		ReductionInfo(ReductionCode soilDensity, ReductionCode pressure,
					  ReductionCode lightExposure, ReductionCode waterUse,
					  ReductionCode soilTemp, ReductionCode envTemp,
					  ReductionCode additive);
		ReductionCode getSoilDensity(void);
		ReductionCode getPressure(void);
		ReductionCode getLightExposure(void);
		ReductionCode getWaterUse(void);
		ReductionCode getSoilTemp(void);
		ReductionCode getEnvTemp(void);
		ReductionCode getAdditive(void);

		friend class Header;
	};

	class PrecisionInfo {
	private:
		float soilDensity;
		float pressure;
		float lightExposure;
		float waterUse;
		float soilTemp;
		float envTemp;
		float additive;
		precision_info_t toCPrecisionInfo(void);

	public:
		PrecisionInfo(void);
		PrecisionInfo(float soilDensity, float pressure, float lightExposure,
					  float waterUse, float soilTemp, float envTemp, float additive);
		float getSoilDensity(void);
		float getPressure(void);
		float getLightExposure(void);
		float getWaterUse(void);
		float getSoilTemp(void);
		float getEnvTemp(void);
		float getAdditive(void);

		friend class Header;
	};

	class Header {
	private:
		FarmingTechnique farmingTec;
		WaveInfo waveInfo;
		SoilDepthInfo soilDepthInfo;
		ReductionInfo reductionInfo;
		PrecisionInfo precisionInfo;
		uint32_t nChunks;
		adf_header_t toCHeader(void);

	public:
		Header(FarmingTechnique farmingTec, WaveInfo waveInfo, SoilDepthInfo depthInfo,
			   ReductionInfo reductionInfo,  PrecisionInfo precisionInfo, uint32_t nChunks);

		FarmingTechnique getFarmingTec(void);
		uint32_t getNChunks(void);
		WaveInfo getWaveInfo(void);
		SoilDepthInfo getDepthInfo(void);
		ReductionInfo getReductionInfo(void);
		PrecisionInfo getPrecisionInfo(void);

		friend class Adf;
	};

	class Additive {
	private:
		uint16_t codeIdx;
		uint32_t code;
		float concentration;
		additive_t toCAdditive(void);

	public:
		Additive(uint32_t code, float concentration);
		Additive(uint16_t codeIdx, uint32_t code, float concentration);
		uint16_t getCodeIdx(void);
		uint32_t getCode(void);
		float getConcentration(void);

		friend class AdditiveList;
	};

	class AdditiveList {
	private:
		std::vector<Additive> additives;
		std::vector<additive_t> cAdditives;
		std::vector<additive_t> toCAdditives(void);

	public:
		AdditiveList();
		AdditiveList(std::vector<Additive> additives);
		size_t size(void);

		friend class Series;
	};

	class Series {
	private:
		Matrix<float> lightExposure;
		Matrix<float> soilTempCelsius;
		std::vector<float> environmentTempCelsius;
		std::vector<float> waterUseMl;
		float pH;
		float pressureBar;
		float soilDensityKgM3;
		AdditiveList soilAdditives;
		AdditiveList atmosphereAdditives;
		uint32_t repeated;
		series_t toCSeries(void);

	public:
		Series(Matrix<float> lightExposure,
			   Matrix<float> soilTemperatureCelsius,
			   std::vector<float> environmenttemperatureCelsius,
			   std::vector<float> wateruseMl, float pH, float pressureBar,
			   float soilDensityKgM3, AdditiveList soilAdditives,
			   AdditiveList atmosphereAdditives, uint32_t repeated);
		Matrix<float> getLightexposure(void);
		Matrix<float> getSoilTemperatureCelsius(void);
		std::vector<float> getEnvironmenttemperatureCelsius(void);
		std::vector<float> getWaterUseMl(void);
		float getPh(void);
		float getPressurebar(void);
		float getSoildensitykgm3(void);
		AdditiveList getSoiladditives(void);
		AdditiveList getAtmosphereadditives(void);
		uint32_t getRepeated(void);

		friend class Adf;
	};

	class Adf {
	private:
		adf_t adf;

	public:
		Adf(Header header, uint32_t periodSec);
		Adf(std::vector<std::byte> bytes);
		~Adf();
		std::string versionString(void);
		Version version(void);
		size_t size(void);
		void addSeries(Series& series);
		void removeSeries(void);
		void updateSeries(Series &series, uint64_t time);
		std::vector<std::byte> marshal(void);
	};
} /* namespace adf */
