/* test_reindex.c
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
#include "test.h"
#include "mock.h"

void series_have_more_additives_than_metadata(void)
{
	uint_t *codes;
	uint16_t res;
	adf_t format;
	series_t *series;

	series = malloc(sizeof(series_t));
	*series = get_series_with_two_soil_additives();

	codes = malloc(sizeof(uint_t));
	*codes = (uint_t){ 2345 };
	
	format = (adf_t) { 
		.header = (adf_header_t) { 
			.signature = { __ADF_SIGNATURE__ },
			.version = __ADF_VERSION__,
			.farming_tec = 3,
			.min_w_len_nm = { 0 },
			.max_w_len_nm = { 10000 },
			.n_chunks = { 10 },
			.n_wavelength = { 10 } 
		},
		.metadata = (adf_meta_t) {
			.period_sec = { 1345 },
			.n_additives = { 1 },
			.size_series = { 1 },
			.n_series = 2,
			.additive_codes = codes 
		},
		.series = series
	};

	res = reindex_additives(&format);
	if (res != ADF_OK) {
		printf("An error occured while reindexing additives [code:%x]\n", res);
		exit(1);
	}

	assert_true(format.metadata.n_additives.val == 2,
				"there are two additives in the metadata section");

	
}

int main(void)
{
	series_have_more_additives_than_metadata();
}
