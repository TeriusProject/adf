#include "adf.hpp"
#include "exceptions.hpp"
#include <vector>
#include <functional>
#include <fstream>

namespace adf {

template<typename T, typename U>
std::vector<U> map(const std::vector<T> &vec, std::function<U(T)> f)
{
	std::vector<U> newVec;
	for (auto const &i : vec) {
		newVec.push_back(f(i));
	}
	return newVec;
}

void checkForErrorAndThrow(uint32_t errorCode)
{
	switch (errorCode) {
		case ADF_HEADER_CORRUPTED:
		case ADF_METADATA_CORRUPTED:
		case ADF_SERIES_CORRUPTED:
		case ADF_ZERO_REPEATED_SERIES:
		case ADF_EMPTY_SERIES:
			throw EmptySeriesException();
		case ADF_RUNTIME_ERROR:
			throw RuntimeException();
		default:
			break;
	}
}

void Adf::addSeries(AdfSeries &series)
{
	series_t cSeries;
	uint32_t res;
	std::function<additive_t(AdfAdditive)> f = [](AdfAdditive add) {
		return (additive_t) {
			.code = {add.getCode()},
			.concentration = {add.getConcentration()}
		};
	};
	
	real_t lightExposureFirstElem = (real_t) { *series.getLightexposure().data() };
	real_t tempCelsiusFirstElem = (real_t) { *series.getTemperaturecelsius().data()};
	real_t waterUseFirstElem = (real_t){
		*series.getWateruseml().data()
	};
	additive_t soilAdditives = (additive_t) { *map(series.getSoiladditives(), f).data() };
	additive_t atmosphereAdditives = (additive_t) { *map(series.getAtmosphereadditives(), f).data() };
	
	cSeries = (series_t) {
		.light_exposure = &lightExposureFirstElem,
		.temp_celsius = &tempCelsiusFirstElem,
		.water_use_ml = &waterUseFirstElem,
		.pH = (uint8_t)(series.getPh() * 10),
		.p_bar = { series.getPressurebar() },
		.soil_density_kg_m3 = { series.getSoildensitykgm3() },
		.n_soil_adds= { (uint16_t)series.getSoiladditives().size() },
		.n_atm_adds = {(uint16_t) series.getAtmosphereadditives().size() },
		.soil_additives = &soilAdditives,
		.atm_additives = &atmosphereAdditives,
		.repeated = {series.getRepeated() }
	};
	
	res = add_series(&this->adf, cSeries);
	checkForErrorAndThrow(res);
}

void Adf::removeSeries(void)
{
	uint32_t res = remove_series(&this->adf);
	checkForErrorAndThrow(res);
}

std::vector<std::byte> Adf::marshal()
{
	std::vector<uint8_t> bytesVec;
	std::function<std::byte(uint8_t)> toCppByte = [] (uint8_t x) {return std::byte{x};};
	size_t adf_size = size_adf_t(this->adf);
	uint8_t *bytes = (uint8_t *)malloc(adf_size * sizeof(uint8_t));
	int res = ::marshal(bytes, this->adf);
	checkForErrorAndThrow(res);
	bytesVec.assign(bytes, bytes + adf_size);
	return map(bytesVec, toCppByte);
}

//static Adf unmarshal(std::vector<std::byte> bytes)
//{
//	
//}

} // namespace adf

int main() {
	adf::AdfHeader header = adf::AdfHeader(1, 10, adf::AdfWaveInfo(15, 350, 1000));
	adf::Adf a = adf::Adf(header, 3600);
	std::vector<float> lightExposure({234,343,5,12,3});
	std::vector<float> temperatureCelsius({23,3,3,2,5});
	std::vector<float> waterUseMl({1,66,3,33,46});
	std::vector<adf::AdfAdditive> soilAdditives({adf::AdfAdditive(1, 16.504)});
	std::vector<adf::AdfAdditive> atmosphereAdditives({adf::AdfAdditive(2, 3.5)});
	
	adf::AdfSeries series(lightExposure,
				   temperatureCelsius,
				   waterUseMl,
				   2.5,
				   3324.67,
				   11.0,
				   soilAdditives,
				   atmosphereAdditives,
				   2);
	a.addSeries(series);
	std::vector<std::byte> arr = a.marshal();
	std::byte h{1};
	
	std::ofstream outfile("/Users/teo/Documents/adf/adf/aaaaaa", std::ios::out | std::ios::binary);
	outfile.write((const char *)&arr[0], arr.size());
	outfile.close();
//	printf("",);
}
