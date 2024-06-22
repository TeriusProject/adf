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

uint32_t fn(void *key) { return *((uint32_t *)key); }

void push_and_pop_one_element(void)
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
				"Push and pop on the same index gets the same value");

	assert_true(t.size == 1, "Table size should be 1");
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

int main()
{
	push_and_pop_one_element();
	push_and_remove_should_be_empty();
	push_and_update();
}
