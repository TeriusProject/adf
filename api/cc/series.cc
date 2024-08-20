/* series.cc
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

	additive_t Additive::toCAdditive(void)
	{
		additive_t add =  {
			/* code_idx should be set by the add_series function */
			.code_idx = { 0 },
			.code = { this->code },
			.concentration = { this->concentration }
		};
		return add;
	}

	Additive::Additive(uint32_t code, float concentration)
		: code(code), concentration(concentration)
	{ }

	Additive::Additive(uint16_t codeIdx, uint32_t code, float concentration)
		: codeIdx(codeIdx), code(code), concentration(concentration)
	{ }

	uint16_t Additive::getCodeIdx(void)
	{
		return this->codeIdx;
	}

	uint32_t Additive::getCode(void)
	{
		return this->code;
	}

	float Additive::getConcentration(void)
	{
		return this->concentration;
	}

	std::vector<additive_t> AdditiveList::toCAdditives(void)
	{
		for (Additive &add : this->additives) {
			additive_t cAdd = add.toCAdditive();
			this->cAdditives.push_back(cAdd);
		}
		return this->cAdditives;
	}

	AdditiveList::AdditiveList() { }

	AdditiveList::AdditiveList(std::vector<Additive> additives)
	{
		this->additives = additives;
		this->toCAdditives();
	}

	size_t AdditiveList::size(void)
	{
		return this->additives.size();
	}

	series_t Series::toCSeries(void)
	{
		return create_series(
			this->lightExposure.startPointer(),
		    this->soilTempCelsius.startPointer(),
			this->environmentTempCelsius.data(),
		    this->waterUseMl.data(),
			(uint8_t)(this->pH * 10),
			this->pressureBar,
			this->soilDensityKgM3,
			(uint16_t) this->soilAdditives.size(),
			(uint16_t) this->atmosphereAdditives.size(),
			reinterpret_cast<additive_t*>(this->soilAdditives.cAdditives.data()),
			reinterpret_cast<additive_t*>(this->atmosphereAdditives.cAdditives.data()),
			this->repeated);
	}

	Series::Series(Matrix<float> lightExposure,
				   Matrix<float> soilTemperatureCelsius,
				   std::vector<float> environmenttemperatureCelsius,
				   std::vector<float> wateruseMl, float pH, float pressureBar,
				   float soilDensityKgM3, AdditiveList soilAdditives,
				   AdditiveList atmosphereAdditives, uint32_t repeated)
		: lightExposure(lightExposure), soilTempCelsius(soilTemperatureCelsius),
		  environmentTempCelsius(environmenttemperatureCelsius),
		  waterUseMl(wateruseMl), pH(pH), pressureBar(pressureBar),
		  soilDensityKgM3(soilDensityKgM3), soilAdditives(soilAdditives),
		  atmosphereAdditives(atmosphereAdditives), repeated(repeated)
	{ }

	Matrix<float> Series::getLightexposure(void)
	{
		return this->lightExposure;
	}

	Matrix<float> Series::getSoilTemperatureCelsius(void)
	{
		return this->soilTempCelsius;
	}

	std::vector<float> Series::getEnvironmenttemperatureCelsius(void)
	{
		return this->environmentTempCelsius;
	}

	std::vector<float> Series::getWaterUseMl(void)
	{
		return this->waterUseMl;
	}

	float Series::getPh(void)
	{
		return this->pH;
	}

	float Series::getPressurebar(void)
	{
		return this->pressureBar;
	}

	float Series::getSoildensitykgm3(void)
	{
		return this->soilDensityKgM3;
	}

	AdditiveList Series::getSoiladditives(void)
	{
		return this->soilAdditives;
	}

	AdditiveList Series::getAtmosphereadditives(void)
	{
		return this->atmosphereAdditives;
	}

	uint32_t Series::getRepeated(void)
	{
		return this->repeated;
	}

} /* namespace adf */
