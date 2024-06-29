/*
 * example.c
 */

#include <adf.h>
#include <stdio.h>
#include <stdlib.h>

#define OUT_FILE_PATH "output.adf"

void write_file(adf_t adf)
{
	FILE *output_file;
	uint8_t *bytes;
	uint16_t res;
	
	bytes = adf_bytes_alloc(adf);
	if((res = marshal(bytes, adf)) != ADF_OK) {
		printf("An error occurred during marshal process [code: %x]\n", res);
		exit(1);
	}
	output_file = fopen(OUT_FILE_PATH, "wb");
	if(!output_file) {
		perror("Error: ");
		exit(1);
   }
	fwrite(bytes, 1, size_adf_t(adf), output_file);
	fclose(output_file);
}

void register_data(adf_t *adf)
{
	printf("Register a new series (enter for default)")
}

int main(void)
{	
	adf_header_t header;
	adf_meta_t metadata;
	adf_t adf;
	
	metadata = (adf_meta_t){ 
		.period_sec = { 21600 }, // each series takes 6 hours
		.n_additives = { 0 },
		.size_series = { 0 },
		.n_series = 0,
		.additive_codes = NULL 
	};
	header = (adf_header_t){ 
		.signature = { __ADF_SIGNATURE__ },
		.version = __ADF_VERSION__,
		.farming_tec = 3,
		.min_w_len_nm = { 0 },
		.max_w_len_nm = { 10000 },
		.n_chunks = { 10 },
		.n_wavelength = { 10 } 
	};
	adf = (adf_t){ 
		.header = header,
		.metadata = metadata,
		.series = NULL 
	};
	
	write_file(adf);
}
