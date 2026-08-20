/* lookup_table.c - Implements a lookup table with linear probing as
 * collision resolution strategy
 * ------------------------------------------------------------------------
 * ADF - Agriculture Data Format
 * Copyright (C) 2024-2026 Matteo Nicoli
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

#include "lookup_table.h"
#include <stdbool.h>
#include <stdio.h>

/*
 * Walks the probe sequence of `key` and returns the slot it's stored in, or
 * the slot it should be inserted into. `*found` tells the two cases apart.
 * It returns false when the sequence is exhausted without finding either,
 * which can only happen on a completely full table.
 */
static bool probe(const table_t *table, uint32_t key, size_t *slot, bool *found)
{
    uint32_t hash = table->hash(&key);
    size_t idx, free_slot = 0;
    bool has_free_slot = false;

    for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
        idx = (hash + i) % max_size;
        if (table->pairs[idx].state == LM_SLOT_BUSY) {
            if (table->pairs[idx].key == key) {
                *slot = idx;
                *found = true;
                return true;
            }
            continue;
        }
        /* A deleted slot can host the key, but the sequence has to go on:
           the key may still be stored further down. A free slot ends it, as
           an insertion would never have jumped over it. */
        if (!has_free_slot) {
            free_slot = idx;
            has_free_slot = true;
        }
        if (table->pairs[idx].state == LM_SLOT_FREE) { break; }
    }

    *slot = free_slot;
    *found = false;
    return has_free_slot;
}

static uint16_t increase_table_size(table_t *table)
{
    size_t old_size = table->max_size, new_size = old_size + table->increment;
    pair_t *old_pairs = table->pairs, *new_pairs;

    if (new_size <= old_size) { return LM_MAP_SIZE_OVERFLOW; }

    new_pairs = calloc(new_size, sizeof(pair_t));
    if (!new_pairs) { return LM_FAILED_EXPANDING_MAP_SIZE; }

    /* The slot of a key is a function of max_size, so growing the table
       invalidates every position: each entry has to be probed into the new
       array rather than copied over. */
    table->pairs = new_pairs;
    table->max_size = new_size;
    for (size_t i = 0; i < old_size; i++) {
        size_t slot;
        bool found;
        if (old_pairs[i].state != LM_SLOT_BUSY) { continue; }
        if (!probe(table, old_pairs[i].key, &slot, &found)) {
            table->pairs = old_pairs;
            table->max_size = old_size;
            free(new_pairs);
            return LM_FAILED_EXPANDING_MAP_SIZE;
        }
        table->pairs[slot] = old_pairs[i];
    }

    free(old_pairs);
    return LM_OK;
}

static bool should_be_resized(table_t *table)
{ return table->size >= table->max_size / 2; }

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

uint16_t table_put(table_t *table, uint32_t key, uint32_t val)
{
    size_t slot;
    bool found;

    if (should_be_resized(table)) {
        uint16_t res = increase_table_size(table);
        if (res != LM_OK) { return res; }
    }

    if (!probe(table, key, &slot, &found)) { return LM_CANNOT_INSERT_VALUE; }

    /* A key already in the table gets its value overwritten: the table holds
       one entry per key, never a duplicate. */
    table->pairs[slot]
        = (pair_t){ .key = key, .value = val, .state = LM_SLOT_BUSY };
    if (!found) { table->size++; }

    return LM_OK;
}

uint16_t table_update(table_t *table, uint32_t key, uint32_t val)
{
    size_t slot;
    bool found;

    if (!probe(table, key, &slot, &found) || !found) {
        return LM_CANNOT_INSERT_VALUE;
    }

    table->pairs[slot].value = val;
    return LM_OK;
}

uint32_t table_get(const table_t *table, uint32_t key)
{
    size_t slot;
    bool found;

    if (!probe(table, key, &slot, &found) || !found) { return 0; }

    return table->pairs[slot].value;
}

uint16_t table_remove(table_t *table, uint32_t key)
{
    size_t slot;
    bool found;

    if (!probe(table, key, &slot, &found) || !found) {
        return LM_CANNOT_REMOVE_NONEXISTENT_VALUE;
    }

    table->pairs[slot]
        = (pair_t){ .key = 0, .value = 0, .state = LM_SLOT_DELETED };
    table->size--;
    return LM_OK;
}

uint16_t table_get_pairs(const table_t *table, pair_t *keys)
{
    size_t counter = 0;

    if (!keys) { return LM_NULL_KEY_ARRAY; }

    for (size_t i = 0, max_size = table->max_size; i < max_size; i++) {
        if (table->pairs[i].state != LM_SLOT_BUSY) { continue; }
        keys[counter] = table->pairs[i];
        counter++;
    }

    return LM_OK;
}

void table_free(table_t *table)
{
    free(table->pairs);
    table->increment = 0;
    table->max_size = 0;
    table->size = 0;
    table->pairs = NULL;
}
