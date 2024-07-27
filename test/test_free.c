/* test_free.c
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
 * ADF is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "../src/adf.h"
#include "mock.h"
#include "test.h"

void test_free_adf(void)
{
	uint16_t res;
	adf_t adf;
	adf_header_t header;
	series_t series;
	
	header = (adf_header_t) {
		.farming_tec = 0x01u,
		.max_w_len_nm = { 1000 },
		.min_w_len_nm = { 0 },
		.n_chunks = { 10 },
		.n_wavelength = { 150 },
		.signature = { __ADF_SIGNATURE__ },
		.version = { __ADF_VERSION__ }
	};
	series = get_series();

	adf_init(&adf, header, 3600);
	res = add_series(&adf, &series);
	if (res != ADF_OK) {
		printf("An error occurred while mocking the series [code: %x]\n", res);
		exit(1);
	}
	adf_free(&adf);
	series_free(&series);

	assert_true(adf.metadata.additive_codes == NULL, 
				"additive_codes is NULL after adf_free is called");
	assert_true(adf.series == NULL, 
				"series array is NULL after adf_free is called");
}

void test_free_series(void)
{
	series_t series = get_series();

	series_free(&series);

	assert_true(series.water_use_ml == NULL, 
				"(series) water_use_ml is NULL after adf_free is called");
	assert_true(series.env_temp_c == NULL, 
				"(series) env_temp_c is NULL after adf_free is called");
	assert_true(series.light_exposure == NULL, 
				"(series) light_exposure is NULL after adf_free is called");
	assert_true(series.soil_additives == NULL, 
				"(series) soil_additives is NULL after adf_free is called");
	assert_true(series.atm_additives == NULL, 
				"(series) atm_additives is NULL after adf_free is called");
}

void test_free_sereies_list(void)
{

}

int main(void)
{
	test_free_adf();
	test_free_series();
	test_free_sereies_list();
}
