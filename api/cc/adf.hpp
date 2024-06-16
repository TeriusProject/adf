#include "../../src/adf.h"
#include <cstdint>
#include <list>

namespace adf {

class AdfHeader {
  private:
	const uint32_t signature = __ADF_SIGNATURE__;
	const uint8_t version = __ADF_VERSION__;
};

class AdfMetadata {
};

class AdfSeries {
};

class Adf {
  private:
	AdfHeader header;
	AdfMetadata metadata;
	std::list<AdfSeries> series;

  public:
	Adf() {}
};

} // namespace adf
