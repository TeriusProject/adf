#include "../src/adf.h"
#include <stdbool.h>
#include <stdio.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define EPSILON 1e-6

void assert_true(bool condition, const char *label);
bool are_ints_equal(uint_t x, uint_t y);
bool are_reals_equal(real_t x, real_t y);
bool are_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size);
bool are_int_arrays_equal(uint_t *x, uint_t *y, uint32_t size);
bool are_real_arrays_equal(real_t *x, real_t *y, size_t size);
void assert_int_equal(uint_t x, uint_t y, const char *label);
void assert_long_equal(long x, long y, const char *label);
void assert_real_equal(real_t x, real_t y, const char *label);
void assert_real_arrays_equal(real_t *x, real_t *y, uint32_t size, const char *label);
void assert_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size, const char *label);
void assert_iter_equal(uint32_t n, adf_t data, iter_t x, iter_t y);
adf_t get_default_object(void);
