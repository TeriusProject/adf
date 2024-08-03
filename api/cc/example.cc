#include <adf.hpp>
#include <cstdio>
#include <fstream>
#include <random>

const std::string fileName = "output.adf";
const uint16_t nSeries = 250;
const uint16_t nChunks = 5;
const uint16_t nWavelength = 15;
const uint16_t nDepth = 2;

adf::Series getRandomSeries()
{
	static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(0, 1);
	adf::Matrix<float> lightExposure(nWavelength);
	adf::Matrix<float> soilTemperature(nDepth);
	std::vector<float> temperatureCelsius;
	std::vector<float> waterUseMl;
	adf::Additive soilAdd(1, 16.504);
	adf::Additive atmAdd(2, 3.5);
	std::vector<adf::Additive> soilAdditives(1, soilAdd);
	std::vector<adf::Additive> atmAdditives(1, atmAdd);
	adf::AdditiveList soilAdditivesList(soilAdditives);
	adf::AdditiveList atmosphereAdditivesList(atmAdditives);
	for (int i = 0; i < nChunks; i++) {
		std::vector<float> lightExposureRow;
		std::vector<float> soilTemperatureRow;

		for (int j = 0; j < nWavelength; j++) {
			lightExposureRow.push_back(dis(e));
		}
		lightExposure.addRow(lightExposureRow);
		for (int j = 0; j < nDepth; j++) {
			soilTemperatureRow.push_back(dis(e));
		}
		soilTemperature.addRow(soilTemperatureRow);
		temperatureCelsius.push_back(dis(e));
		waterUseMl.push_back(dis(e));
	}
	
	adf::Series series(lightExposure,
					   soilTemperature,
					   temperatureCelsius,
					   waterUseMl,
					   2.5,
					   3324.67,
					   11.0,
					   soilAdditivesList,
					   atmosphereAdditivesList,
					   1);
	return series;
}

void updateData(adf::Adf& adf)
{
	for (int i = 0; i < nSeries; i++) {
		adf::Series s = getRandomSeries();
		adf.addSeries(s);
	}
}

void createAdfAndSaveToFile(void)
{
	adf::Header header = adf::Header(
		adf::FarmingTechnique::Fogponics,
		adf::WaveInfo(nWavelength, 350, 1000),
		adf::SoilDepthInfo(nDepth,0,0),
		adf::ReductionInfo(),
		nChunks);
	adf::Adf adf = adf::Adf(header, ADF_DAY);
	updateData(adf);
	std::vector<std::byte> bytes = adf.marshal();
	
	std::ofstream fs(fileName, std::ios::binary);
	fs.write(reinterpret_cast<char*>(bytes.data()), adf.size());
	fs.close();
	std::cout << "Current ADF version: " << adf.versionString() << std::endl;
	std::cout << "Wrote ADF file (" << adf.size() << " bytes)\nfilename: " << fileName << std::endl;
}

std::vector<std::byte> readFile(void) 
{
	std::ifstream in(fileName, std::ios::binary);
    in.seekg(0, std::ios_base::end);
    auto length = in.tellg();
    in.seekg(0, std::ios_base::beg);
    std::vector<std::byte> buffer(length);
    in.read(reinterpret_cast<char*>(buffer.data()), length);
    in.close();
	std::cout << "Read file "<< fileName << " (" << length << " bytes)" << std::endl;
	return buffer;
}

void readFileAndGenerateAdf(void)
{
	std::vector<std::byte> bytes = readFile();
	adf::Adf adf(bytes);
	std::cout << "ADF size: " << adf.size() << " bytes" << std::endl;
}

int main(void) 
{
	createAdfAndSaveToFile();
	readFileAndGenerateAdf();
	std::cout << "*DONE*" << std::endl;
}
