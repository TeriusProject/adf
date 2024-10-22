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
 * ADF is distributed in the hope that it will be useful,
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
	uint16_t res;
	uint32_t value, returned_value;
	table_t t;

	res = table_init(&t, 256, 256, &fn);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}
	value = 1234;
	res = table_put(&t, 13, value);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}
	returned_value = table_get(&t, 13);

	assert_true(value == returned_value,
				"Push and pop the same integer");
	assert_true(t.size == 1, "One integer inserted, table size should be 1");

	table_free(&t);
}

void push_and_remove_should_be_empty(void)
{
	uint16_t res;
	uint32_t value;
	table_t t;

	res = table_init(&t, 256, 256, &fn);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}
	value = 1234;
	res = table_put(&t, 13, value);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}
	res = table_remove(&t, 13);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}

	assert_true(t.size == 0, "Table size should be 0");

	table_free(&t);
}

void push_and_update(void)
{
	uint16_t res;
	uint32_t value, new_value, returned_value;
	table_t t;

	res = table_init(&t, 256, 256, &fn);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}
	value = 1234;
	res = table_put(&t, 13, value);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}
	new_value = 5678;
	res = table_update(&t, 13, new_value);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}
	returned_value = (uint32_t) table_get(&t, 13);

	assert_true(new_value == returned_value,
				"Table should retur the updated value");
	
	table_free(&t);
}

void map_should_resize_when_half_full(void)
{
	uint16_t res;
	uint32_t rnd_number;
	table_t t;
	
	res = table_init(&t, 16, 16, &fn);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}
	for (uint8_t i = 0, l = (uint8_t)(t.max_size / 2) + 2; i < l; i++) {
		rnd_number = rand();
		res = table_put(&t, i, rnd_number);
		if (res != LM_OK) {
			printf("[%x] %s", res, "An error occurred\n");
			exit(1);
		}
	}

	assert_true(t.size > 8 && t.size < 16,
				"Table should contain less than 16 keys, but more than 8");
	assert_true(t.max_size == 32, "Table capacity should be doubled");

	table_free(&t);
}

void table_keys_should_return_the_list_of_the_inserted_keys(void)
{
	uint16_t res;
	uint32_t rnd_number;
	pair_t *expected_keys, *keys;
	table_t t;
	
	res = table_init(&t, 16, 16, &fn);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}

	expected_keys = malloc(((t.max_size / 2) + 2) * sizeof(pair_t));
	for (uint8_t i = 0, l = (uint8_t)(t.max_size / 2) + 2; i < l; i++) {
		rnd_number = rand();
		res = table_put(&t, i, rnd_number);
		if (res != LM_OK) {
			printf("[%x] %s", res, "An error occurred\n");
			exit(1);
		}
		expected_keys[i] = (pair_t){ .key = i, .value = 0 };
	}
	keys = malloc(t.size * sizeof(pair_t));
	if((res = table_get_pairs(&t, keys)) != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}

	assert_pair_arrays_equal(
		keys, expected_keys, t.size,
		"table_get_pairs should return the available pairs");
	
	free(keys);
	free(expected_keys);
	table_free(&t);
}

void test_free_lookup_table(void)
{
	uint8_t size = 250;
	uint16_t res;
	table_t t;
	uint32_t *nums;

	res = table_init(&t, 1024, 1024, &fn);
	if (res != LM_OK) {
		printf("[%x] %s", res, "An error occurred\n");
		exit(1);
	}
	nums = malloc(size * sizeof(uint32_t));
	for (uint8_t i = 0; i < size; i++) {
		nums[i] = rand();
		res = table_put(&t, 13, nums[i]);
		if (res != LM_OK) {
			printf("[%x] %s", res, "An error occurred\n");
			exit(1);
		}
	}

	assert_true(t.size == 250, "table has size equal to 250");
	table_free(&t);
	assert_true(t.size == 0, "After free table has size equal to 0");
	assert_true(t.max_size == 0, "After free table has max_size equal to 0");
	assert_true(t.increment == 0, "After free table has increment equal to 0");
	assert_true(t.pairs == NULL, "table has pairs equal to NULL");
}


int main(void)
{
	srand(time(NULL));
	push_and_pop_one_integer();
	push_and_remove_should_be_empty();
	push_and_update();
	map_should_resize_when_half_full();
	table_keys_should_return_the_list_of_the_inserted_keys();
	test_free_lookup_table();
}
