#include "../src/adf.h"
#include <stdbool.h>
#include <stdio.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define EPSILON 1e-11

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

bool are_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		if (x[i] != y[i])
			return false;

	return true;
}

bool are_int_arrays_equal(uint_t *x, uint_t *y, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		if (!are_ints_equal(x[i], y[i]))
			return false;

	return true;
}

bool are_real_arrays_equal(real_t *x, real_t *y, size_t size)
{
	for (size_t i = 0; i < size; i++)
		if (!are_reals_equal(x[i], y[i]))
			return false;

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

void assert_real_arrays_equal(real_t *x, real_t *y, uint32_t size, const char *label)
{
	assert_true(are_real_arrays_equal(x, y, size), label);
}

void assert_uint8_arrays_equal(uint8_t *x, uint8_t *y, uint32_t size, const char *label)
{
	assert_true(are_uint8_arrays_equal(x, y, size), label);
}

void assert_iter_equal(uint32_t n, adf_t data, iter_t x, iter_t y, const char *label)
{
	printf("(iteration %u)\n", n);
	assert_real_arrays_equal(x.light_exposure, y.light_exposure, data.n_chunks.val, "are light_exposures equal");
	assert_real_arrays_equal(x.temp_celsius, y.temp_celsius, data.n_chunks.val, "are temp_celsiuss equal");
	assert_real_arrays_equal(x.water_use_ml, y.water_use_ml, data.n_chunks.val, "are water_use_mls equal");
	assert_uint8_arrays_equal(x.light_wavelength, y.light_wavelength, data.n_wavelength.val, "are light_wavelengths equal");
	assert_real_equal(x.pH, y.pH, "are pHs equal");
	assert_real_equal(x.pressure_pa, y.pressure_pa, "are pressure_pas equal");
	assert_real_equal(x.soil_density_t_m3, y.soil_density_t_m3, "are soil_density_t_m3s equal");
	assert_real_equal(x.nitrogen_g_m3, y.nitrogen_g_m3, "are nitrogen_g_m3s equal");
	assert_real_equal(x.potassium_g_m3, y.potassium_g_m3, "are potassium_g_m3s equal");
	assert_real_equal(x.phosphorus_g_m3, y.phosphorus_g_m3, "are phosphorus_g_m3s equal");
	assert_real_equal(x.iron_g_m3, y.iron_g_m3, "are iron_g_m3s equal");
	assert_real_equal(x.magnesium_g_m3, y.magnesium_g_m3, "are magnesium_g_m3s equal");
	assert_real_equal(x.sulfur_g_m3, y.sulfur_g_m3, "are sulfur_g_m3s equal");
	assert_real_equal(x.calcium_g_m3, y.calcium_g_m3, "are calcium_g_m3s equal");
}
