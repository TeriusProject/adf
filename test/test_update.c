/*
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
 * Teriusis distributed in the hope that it will be useful,
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
#include <stdio.h>
#include <stdlib.h>

int main() {
	series_t series = get_series();
	adf_t adf = get_default_object();
	int res = add_series(&adf, series);
	(void) res;
	uint_t expected_n_series = {3};
	printf("%d\n",adf.metadata.n_series.val);
	assert_int_equal(adf.metadata.n_series, expected_n_series, "are n_series equal");
}