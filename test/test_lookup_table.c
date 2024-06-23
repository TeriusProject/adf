/* test_lookup_table.c
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

#include "../src/lookup_table.h"
#include "test.h"
#include <stdio.h>
#include <time.h>

uint32_t fn(void *key) { return *((uint32_t *)key); }

void push_and_pop_one_integer(void)
{
	table_t t;
	uint16_t res = table_init(&t, 256, 256, &fn);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	uint32_t value = 1234;
	res = table_put(&t, 13, &value);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	uint32_t *returned_value = (uint32_t *)table_get(&t, 13);
	assert_true(value == *returned_value,
				"Push and pop the same integer");

	assert_true(t.size == 1, "One integer inserted, table size should be 1");
}

void push_and_remove_should_be_empty(void)
{
	table_t t;
	uint16_t res = table_init(&t, 256, 256, &fn);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	uint32_t value = 1234;
	res = table_put(&t, 13, &value);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	res = table_remove(&t, 13);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	assert_true(t.size == 0, "Table size should be 0");
}

void push_and_update(void)
{
	table_t t;
	uint16_t res = table_init(&t, 256, 256, &fn);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	uint32_t value = 1234;
	res = table_put(&t, 13, &value);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	uint32_t new_value = 5678;
	res = table_update(&t, 13, &new_value);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	uint32_t *returned_value = (uint32_t *)table_get(&t, 13);
	assert_true(new_value == *returned_value,
				"Table should retur the updated value");
}

void map_should_resize_when_half_full(void)
{
	table_t t;
	uint16_t res = table_init(&t, 16, 16, &fn);
	int rnd_number;
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	for (uint8_t i = 0, l = (uint8_t)(t.max_size / 2) + 2; i < l; i++) {
		rnd_number = rand();
		res = table_put(&t, i, &rnd_number);
		if (res != LM_OK) {
			printf("[%hu] %s", res, "An error occurred\n");
			exit(1);
		}
	}
	assert_true(t.size > 8 && t.size < 16,
				"Table should contain less than 16 keys, but more than 8");
	assert_true(t.max_size == 32, "Table capacity should be doubled");
}

void table_keys_should_return_the_list_of_the_inserted_keys(void)
{
	table_t t;
	uint16_t res = table_init(&t, 16, 16, &fn);
	int rnd_number;
	pair_t *expected_keys = malloc(((t.max_size / 2) + 2) * sizeof(pair_t)),
		   *keys;
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	for (uint8_t i = 0, l = (uint8_t)(t.max_size / 2) + 2; i < l; i++) {
		rnd_number = rand();
		res = table_put(&t, i, &rnd_number);
		if (res != LM_OK) {
			printf("[%hu] %s", res, "An error occurred\n");
			exit(1);
		}
		expected_keys[i] = (pair_t){ .key = i, .value = NULL };
	}
	keys = table_get_pairs(&t);
	assert_pair_arrays_equal(
		keys, expected_keys, t.size,
		"table_get_pairs should return the available pairs");
}

void push_and_pop_one_object(void)
{
	table_t t;
	uint16_t res = table_init(&t, 256, 256, &fn);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	additive_t value = (additive_t) {
		.code = {1234},
		.code_idx = {12},
		.concentration = {0.56789}
	};
	res = table_put(&t, 13, &value);
	if (res != LM_OK) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	
	additive_t *returned_value = (additive_t *)table_get(&t, 13);
	if (!returned_value) {
		printf("[%hu] %s", res, "An error occurred\n");
		exit(1);
	}
	assert_true(are_additives_equal(value, *returned_value),
				"Push and pop the same object");
	assert_true(t.size == 1,  "One object inserted, table size should be 1");
}

int main(void)
{
	srand(time(NULL));
	printf("(Lookup table test)\n");
	push_and_pop_one_integer();
	push_and_pop_one_object();
	push_and_remove_should_be_empty();
	push_and_update();
	map_should_resize_when_half_full();
	table_keys_should_return_the_list_of_the_inserted_keys();
}
