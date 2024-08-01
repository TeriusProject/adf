/* test_create.c
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

void test_init_adf(void)
{
	adf_t adf;
	adf_header_t header = get_default_header();
	adf_init(&adf, header, 3600);
	assert_header_equal(adf.header, header, "headers should be equal");
	assert_true(!adf.series, "series should be initialized to NULL");
	assert_true(!adf.metadata.additive_codes,
				"additive_codes should be initialized to NULL");
}

int main(void)
{
	test_init_adf();
}
