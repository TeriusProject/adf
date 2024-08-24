/* header.cc - Implementation of the C++ interface for the ADF library
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

namespace adf {

	wavelength_info_t WaveInfo::toCWaveInfo(void)
	{
		return create_wavelength_info(this->maxWlenNm,
									  this->minWlenNm,
									  this->nWavelength);
	}

	WaveInfo::WaveInfo(uint16_t nWavelength, uint16_t minWlenNm, uint16_t maxWlenNm)
		: nWavelength(nWavelength), minWlenNm(minWlenNm), maxWlenNm(maxWlenNm)
	{ }

	uint16_t WaveInfo::getNWavelength(void)
	{
		return this->nWavelength;
	}

	uint16_t WaveInfo::getMinWlenNm(void)
	{
		return this->minWlenNm;
	}

	uint16_t WaveInfo::getMaxWlenNm(void)
	{
		return this->maxWlenNm;
	}

	soil_depth_info_t SoilDepthInfo::toCSoilInfo(void)
	{
		return create_trans_soil_depth_info(this->maxDepthMm,
											this->nDepthMeasurements,
											this->tY);
	}

	SoilDepthInfo::SoilDepthInfo(uint16_t nDepthMeasurements, uint16_t tY,
								 uint16_t maxDepthMm)
		: nDepthMeasurements(nDepthMeasurements), tY(tY),
		  maxDepthMm(maxDepthMm)
	{ }

	uint16_t SoilDepthInfo::getNDepthMeasurements(void)
	{
		return this->nDepthMeasurements;
	}

	uint16_t SoilDepthInfo::getTY(void)
	{
		return this->tY;
	}

	uint16_t SoilDepthInfo::getMaxDepthMm(void)
	{
		return this->maxDepthMm;
	}

	reduction_info_t ReductionInfo::toCReductionInfo(void)
	{
		return create_reduction_info(static_cast<reduction_code_t>(this->soilDensity),
									 static_cast<reduction_code_t>(this->pressure),
									 static_cast<reduction_code_t>(this->lightExposure),
									 static_cast<reduction_code_t>(this->waterUse),
									 static_cast<reduction_code_t>(this->soilTemp),
									 static_cast<reduction_code_t>(this->envTemp),
									 static_cast<reduction_code_t>(this->additive));
	}

	ReductionInfo::ReductionInfo(void)
		: soilDensity(ReductionCode::Average), pressure(ReductionCode::Average),
		  lightExposure(ReductionCode::Average), waterUse(ReductionCode::Average),
		  soilTemp(ReductionCode::Average), envTemp(ReductionCode::Average),
		  additive(ReductionCode::Average)
	{ }

	ReductionInfo::ReductionInfo(ReductionCode soilDensity, ReductionCode pressure, ReductionCode lightExposure,
								 ReductionCode waterUse, ReductionCode soilTemp, ReductionCode envTemp,
								 ReductionCode additive)
		: soilDensity(soilDensity), pressure(pressure),
		  lightExposure(lightExposure), waterUse(waterUse),
		  soilTemp(soilTemp), envTemp(envTemp), additive(additive)
	{ }

	ReductionCode ReductionInfo::getSoilDensity(void)
	{
		return this->soilDensity;
	}

	ReductionCode ReductionInfo::getPressure(void)
	{
		return this->pressure;
	}

	ReductionCode ReductionInfo::getLightExposure(void)
	{
		return this->lightExposure;
	}

	ReductionCode ReductionInfo::getWaterUse(void)
	{
		return this->waterUse;
	}

	ReductionCode ReductionInfo::getSoilTemp(void)
	{
		return this->soilTemp;
	}


	ReductionCode ReductionInfo::getEnvTemp(void)
	{
		return this->envTemp;
	}

	ReductionCode ReductionInfo::getAdditive(void)
	{
		return this->additive;
	}

	precision_info_t PrecisionInfo::toCPrecisionInfo(void)
	{
		return create_precision_info(this->soilDensity,
									 this->pressure,
									 this->lightExposure,
									 this->waterUse,
									 this->soilTemp,
									 this->envTemp,
									 this->additive);
	}

	PrecisionInfo::PrecisionInfo(void)
		: soilDensity(0.0), pressure(0.0), lightExposure(0.0), waterUse(0.0),
		  soilTemp(0.0), envTemp(0.0), additive(0.0)
	{ }

	PrecisionInfo::PrecisionInfo(float soilDensity, float pressure, float lightExposure,
								 float waterUse, float soilTemp, float envTemp, float additive)
		: soilDensity(soilDensity), pressure(pressure), lightExposure(lightExposure),
		  waterUse(waterUse), soilTemp(soilTemp), envTemp(envTemp), additive(additive)
	{ }

	float PrecisionInfo::getSoilDensity(void) { return this->soilDensity; }
	float PrecisionInfo::getPressure(void) { return this->pressure; }
	float PrecisionInfo::getLightExposure(void) { return this->lightExposure; }
	float PrecisionInfo::getWaterUse(void) { return this->waterUse; }
	float PrecisionInfo::getSoilTemp(void) { return this->soilTemp; }
	float PrecisionInfo::getEnvTemp(void) { return this->envTemp; }
	float PrecisionInfo::getAdditive(void) { return this->additive; }

	adf_header_t Header::toCHeader(void)
	{
		return create_header(
			static_cast<uint8_t>(this->farmingTec),
			this->waveInfo.toCWaveInfo(),
			this->soilDepthInfo.toCSoilInfo(),
			this->reductionInfo.toCReductionInfo(),
			this->precisionInfo.toCPrecisionInfo(),
			this->nChunks
			);
	}

	Header::Header(FarmingTechnique farmingTec, WaveInfo waveInfo, SoilDepthInfo depthInfo,
				   ReductionInfo reductionInfo, PrecisionInfo precisionInfo, uint32_t nChunks)
		: farmingTec(farmingTec), waveInfo(waveInfo), soilDepthInfo(depthInfo),
		  reductionInfo(reductionInfo), precisionInfo(precisionInfo), nChunks(nChunks)
	{ }

	FarmingTechnique Header::getFarmingTec(void)
	{
		return this->farmingTec;
	}

	uint32_t Header::getNChunks(void)
	{
		return this->nChunks;
	}

	WaveInfo Header::getWaveInfo(void)
	{
		return this->waveInfo;
	}

	SoilDepthInfo Header::getDepthInfo(void)
	{
		return this->soilDepthInfo;
	}

	ReductionInfo Header::getReductionInfo(void)
	{
		return this->reductionInfo;
	}

	PrecisionInfo Header::getPrecisionInfo(void)
	{
		return this->precisionInfo;
	}

} /* namespace adf */
