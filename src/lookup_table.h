/*
 * lookup_table.h
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

#ifndef __LOOKUP_TABLE_H__
#define __LOOKUP_TABLE_H__

#include <stdlib.h>
#include <stdint.h>

#define DEFAULT_MAP_SIZE 1024

typedef enum {
	LM_OK = 0x00u,
	LM_CANNOT_INIT_TABLE = 0x01u,
	LM_CANNOT_INIT_TABLE_PAIRS = 0x02u,
	LM_CANNOT_INSERT_VALUE = 0x03u,
	LM_NULL_VALUE_NOT_ALLOWED = 0x04u,
	LM_VALUE_NOT_FOUND = 0x05u,
	LM_MAP_SIZE_OVERFLOW = 0x06u,
	LM_FAILED_EXPANDING_MAP_SIZE = 0x07u,
} table_code;

typedef uint32_t (*hash_fn_t)(void *);

typedef struct {
	uint32_t key;
	void *value;
} pair_t;

typedef struct {
	pair_t *pairs;
	size_t max_size;
	size_t increment;
    size_t size;
	hash_fn_t hash;
} table_t;

uint16_t table_init(table_t *, size_t, size_t, hash_fn_t);
uint16_t table_put(table_t *, uint32_t, void *);
uint16_t table_update(table_t *,uint32_t, void *);
uint16_t table_remove(table_t *, uint32_t);
void *table_get(const table_t *, uint32_t);
pair_t *table_get_pairs(const table_t *);
void pair_free(pair_t *);
void table_free(table_t *);

#endif /* __LOOKUP_TABLE_H__ */
