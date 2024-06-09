#include <stdlib.h>
#include <stdbool.h>

bool binaryFind(double x, double *array, size_t N) {
    size_t lower = 0;
    size_t upper = N;
	size_t mid;

    while (lower + 1 < upper) {
        mid = (lower + upper) / 2;
        if (x < array[mid]) {
            upper = mid;
        } else {
            lower = mid;
        }
    }
    if (array[lower] <= x) return lower;
    return (size_t)-1;
}
