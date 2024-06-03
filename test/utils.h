#include "../src/adf.h"
#include <stdbool.h>
#include <stdio.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define EPSILON 1e-6

void assert_true(bool condition, const char *label)
{
	if (condition)
		printf(GREEN "PASSED (\u2713): %s\n" RESET, label);
	else
		printf(RED "FAILED (\u2A09): %s\n" RESET, label);
}

bool are_ints_equal(uint_t x, uint_t y)
{
	return x.val == y.val;
}

bool are_reals_equal(real_t x, real_t y)
{
	return x.val < y.val
			   ? (y.val - x.val) < EPSILON
			   : (x.val - y.val) < EPSILON;
}

bool are_int_arrays_equal(uint_t *x, uint_t *y, size_t size)
{
	for (size_t i = 0; i < size; i++)
		if (!are_ints_equal(x[i], y[i])) return false;
	
	return true;
}

bool are_real_arrays_equal(real_t *x, real_t *y, size_t size)
{
	for (size_t i = 0; i < size; i++)
		if (!are_reals_equal(x[i], y[i])) return false;
	
	return true;
}

void assert_int_equal(uint_t x, uint_t y, const char *label)
{
	assert_true(x.val == y.val, label);
}

void assert_real_equal(real_t x, real_t y, const char *label)
{
	assert_true(are_reals_equal(x, y), label);
}

void assert_iter_equal(adf_t data,iter_t x, iter_t y, const char *label)
{
	
	bool reals = are_reals_equal(x.pH, y.pH) &
				 are_reals_equal(x.pressure_pa, y.pressure_pa) &
				 are_reals_equal(x.soil_density_t_m3, y.soil_density_t_m3) &
				 are_reals_equal(x.nitrogen_g_m3, y.nitrogen_g_m3) &
				 are_reals_equal(x.potassium_g_m3, y.potassium_g_m3) &
				 are_reals_equal(x.phosphorus_g_m3, y.phosphorus_g_m3) &
				 are_reals_equal(x.iron_g_m3, y.iron_g_m3) &
				 are_reals_equal(x.magnesium_g_m3, y.magnesium_g_m3) &
				 are_reals_equal(x.sulfur_g_m3, y.sulfur_g_m3) &
				 are_reals_equal(x.calcium_g_m3, y.calcium_g_m3);
	
}
