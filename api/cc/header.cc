/* 
 * header.cc - Implementation of the C++ interface for the ADF library
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
	wavelength_info_t w_info = {
		.max_w_len_nm = { this->maxWlenNm },
		.min_w_len_nm = { this->minWlenNm },
		.n_wavelength = { this->nWavelength },
	};
	return w_info;
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
	soil_depth_info_t s_info = {
		.n_depth = { this->nDepthMeasurements },
		.t_y = { this->minDepthMm },
		.max_soil_depth_mm = { this->maxDepthMm }
	};
	return s_info;
}

SoilDepthInfo::SoilDepthInfo(uint16_t nDepthMeasurements, uint16_t minDepthMm,
							 uint16_t maxDepthMm)
	: nDepthMeasurements(nDepthMeasurements), minDepthMm(minDepthMm),
	maxDepthMm(maxDepthMm)
{ }

uint16_t SoilDepthInfo::getNDepthMeasurements(void)
{
	return this->nDepthMeasurements;
}

uint16_t SoilDepthInfo::getMinDepthMm(void)
{
	return this->minDepthMm;
}

uint16_t SoilDepthInfo::getMaxDepthMm(void)
{
	return this->maxDepthMm;
}

reduction_info_t ReductionInfo::toCReductionInfo(void)
{
	reduction_info_t r_info = {
		.soil_density_red_mode = static_cast<reduction_code_t>(this->soilDensity),
		.pressure_red_mode = static_cast<reduction_code_t>(this->pressure),
		.light_exposure_red_mode = static_cast<reduction_code_t>(this->lightExposure),
		.water_use_red_mode = static_cast<reduction_code_t>(this->waterUse),
		.soil_temp_red_mode = static_cast<reduction_code_t>(this->soilTemp),
		.env_temp_red_mode = static_cast<reduction_code_t>(this->envTemp)
	};
	return r_info;
}

ReductionInfo::ReductionInfo()
	: soilDensity(ReductionCode::Average), pressure(ReductionCode::Average),
		lightExposure(ReductionCode::Average), waterUse(ReductionCode::Average),
		soilTemp(ReductionCode::Average), envTemp(ReductionCode::Average)
{ }

ReductionInfo::ReductionInfo(ReductionCode soilDensity, ReductionCode pressure, ReductionCode lightExposure,
				ReductionCode waterUse, ReductionCode soilTemp, ReductionCode envTemp)
	: soilDensity(soilDensity), pressure(pressure),
		lightExposure(lightExposure), waterUse(waterUse),
		soilTemp(soilTemp), envTemp(envTemp) 
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

adf_header_t Header::toCHeader(void)
{
	return create_header(
		static_cast<uint8_t>(this->farmingTec),
		this->waveInfo.toCWaveInfo(),
		this->soilDepthInfo.toCSoilInfo(),
		this->reductionInfo.toCReductionInfo(),
		this->nChunks
	);
}

Header::Header(FarmingTechnique farmingTec, WaveInfo waveInfo, SoilDepthInfo depthInfo,
		   ReductionInfo reductionInfo, uint32_t nChunks)
	: farmingTec(farmingTec), waveInfo(waveInfo), soilDepthInfo(depthInfo),
		reductionInfo(reductionInfo), nChunks(nChunks)
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

} /* namespace adf */
