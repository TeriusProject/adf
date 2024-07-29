/* mock.h
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

#ifndef __MOCK_H__
#define __MOCK_H__

#include "../src/adf.h"

real_t *get_real_array(int);
real_t **get_real_matrix(int, int);
series_t get_series(void);
series_t get_repeated_series(void);
adf_t get_default_object(void);
adf_t get_object_with_zero_series(void);
series_t get_series_with_two_soil_additives(void);

#endif /* __MOCK_H__ */
