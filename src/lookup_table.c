/*
 * lookup_table.c - Implements a lookup table with linear probing as 
 * collision resolution strategy
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
 * Terius is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdbool.h>
#include "lookup_table.h"

static uint16_t increase_table_size(table_t *table)
{
	size_t old_size = table->max_size,
		   new_size = old_size + table->increment;

    if (new_size <= old_size) { return LM_MAP_SIZE_OVERFLOW; }
	
	table->pairs = realloc(table->pairs, new_size * sizeof(pair_t));
	if (!table->pairs) { return LM_FAILED_EXPANDING_MAP_SIZE; }

	for (size_t i = old_size; i < new_size; i++) {
		table->pairs[i].key = 0;
		table->pairs[i].value = NULL;
	}

	table->max_size = new_size;
	return LM_OK;
}

static bool should_be_resized(table_t *table)
{
	return table->size >= table->max_size / 2;
}

uint16_t table_init(table_t *table, size_t capacity, size_t increment,
					hash_fn_t hash)
{
	if (!table) { return LM_CANNOT_INIT_TABLE; }
	
	table->size = 0;
    table->max_size = capacity;
	table->increment = increment;
	table->pairs = calloc(capacity, sizeof(pair_t));
	table->hash = hash;
    if (table->pairs == NULL) { return LM_CANNOT_INIT_TABLE_PAIRS; }
	return LM_OK;
}

uint16_t table_put(table_t *table, uint32_t key, void *val)
{
	uint32_t hash;
    size_t idx;

	if (!val) { return LM_NULL_VALUE_NOT_ALLOWED; }

	if (should_be_resized(table)) {
		uint16_t res = increase_table_size(table);
        if (res != LM_OK) { return res; }
    }

	hash = table->hash(&key);

	for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
		idx = (hash + i) % (max_size - 1);
		if (!table->pairs[idx].value) {
			table->pairs[idx] = (pair_t) {.key = idx, .value = val};
			table->size++;
			return LM_OK;
		}
	}
    return LM_CANNOT_INSERT_VALUE;
}

uint16_t table_update(table_t *table, uint32_t key, void *val)
{
	uint32_t hash;
    size_t idx;

	hash = table->hash(&key);

	for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
		idx = (hash + i) % (max_size - 1);
		if (table->pairs[idx].key == key) {
			table->pairs[idx].value = val;
			return LM_OK;
		}
	}
    return LM_CANNOT_INSERT_VALUE;
}

void *table_get(const table_t * table, uint32_t key)
{
    uint16_t hash;
    size_t idx;

	hash = table->hash(&key);
	for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
		idx = (hash + i) % (max_size - 1);
		if (table->pairs[idx].key == key) {
			return table->pairs[idx].value;
		}
	}
	return NULL;
}

uint16_t table_remove(table_t * table, uint32_t key)
{
    uint16_t res = table_update(table, key, NULL);
	if (res != LM_OK) { return res; }
	table->size--;
	return LM_OK;
}

pair_t *table_get_pairs(const table_t *table)
{
	pair_t *keys = malloc(table->size * sizeof(pair_t));
	size_t counter = 0;

	for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
		if (table->pairs[i].value) {
			keys[counter] = table->pairs[i];
			counter++;
		}
	}

	return keys;
}

void pair_free(pair_t *pair)
{
	free(pair->value);
}

void table_free(table_t *table)
{
    for (size_t i = 0, l = table->max_size; i < l; i++)
		pair_free(table->pairs + i);

    free(table->pairs);
    free(table);
}
