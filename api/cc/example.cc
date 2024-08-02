#include <adf.hpp>
#include <cstdio>
#include <fstream>
#include <random>

#define FILE_NAME "output.adf"
#define N_SERIES 250
#define N_CHUNKS 5
#define N_WAVELENGTH 15
#define N_DEPTH 2

adf::Series getRandomSeries()
{
	std::default_random_engine e;
    std::uniform_real_distribution<> dis(0, 1);
	adf::Matrix<float> lightExposure(N_CHUNKS, N_WAVELENGTH);
	adf::Matrix<float> soilTemperature(N_CHUNKS, N_DEPTH);
	std::vector<float> temperatureCelsius;
	std::vector<float> waterUseMl;
	std::vector<adf::Additive> soilAdditives({adf::Additive(1, 16.504)});
	std::vector<adf::Additive> atmosphereAdditives({adf::Additive(2, 3.5)});

	for (int i = 0; i < N_CHUNKS; i++) {
		std::vector<float> lightExposureRow;
		std::vector<float> soilTemperatureRow;

		for (int j = 0; j < N_WAVELENGTH; j++) {
			lightExposureRow.push_back(dis(e));
		}
		lightExposure.addRow(lightExposureRow);
		for (int j = 0; j < N_DEPTH; j++) {
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
					   soilAdditives,
					   atmosphereAdditives,
					   1);
	return series;
}

void updateData(adf::Adf& adf)
{
	for (int i = 0; i < N_SERIES; i++) {
		adf::Series s = getRandomSeries();
		adf.addSeries(s);
	}
}

void createAdfAndSaveToFile(void)
{
	adf::Header header = adf::Header(
		ADF_FT_REGULAR,
		adf::WaveInfo(N_WAVELENGTH, 350, 1000),
		adf::SoilDepthInfo(N_DEPTH,0,0),
		adf::ReductionInfo(),
		N_CHUNKS);
	adf::Adf adf = adf::Adf(header, ADF_DAY);
	updateData(adf);
	std::vector<std::byte> bytes = adf.marshal();
	std::ofstream fs(FILE_NAME, std::ios::out | std::ios::binary);
	fs.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
	fs.close();
	std::cout << "Current ADF version: " << adf.versionString() << std::endl;
	std::cout << "Wrote ADF file (" << adf.size() << " bytes)\nfilename: " << FILE_NAME << std::endl;
}

std::vector<std::byte> readFile(void) 
{
	std::ifstream in(FILE_NAME, std::ios_base::binary);
    in.seekg(0, std::ios_base::end);
    auto length = in.tellg();
    in.seekg(0, std::ios_base::beg);
    std::vector<std::byte> buffer(length);
    in.read(reinterpret_cast<char*>(buffer.data()), length);
    in.close();
	std::cout << "Read file "<< FILE_NAME << " (" << length << " bytes)" << std::endl;
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
