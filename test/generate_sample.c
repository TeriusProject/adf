#include "../src/adf.h"
#include "mock.h"
#include <stdio.h>
#include <stdlib.h>

#define FILE_PATH "sample.adf"

int main(void)
{
	uint16_t res;
	adf_t expected = get_default_object();
	uint8_t *bytes = malloc(size_adf_t(&expected) * sizeof(uint8_t));
	if((res = marshal(bytes, &expected)) != ADF_OK) {
		printf("[%x] %s", res, "An error occurred during marshal process\n");
		exit(1);
	}
	FILE *sample_file = fopen(FILE_PATH, "wb");
	fwrite(bytes, 1, size_adf_t(&expected), sample_file);
	fclose(sample_file);
}
