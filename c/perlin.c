/* perlin.c - Classical Perlin Noise in 2D
 * Implementation as found in: https://en.wikipedia.org/wiki/Perlin_noise
 * Needs an update; this is missing out on improvements made later by Ken Perlin.
 * https://developer.nvidia.com/gpugems/gpugems/part-i-natural-effects/chapter-5-implementing-improved-perlin-noise
 */

#include <math.h>
#include <stdlib.h>
#include "perlin.h"

/* Collection of pre-calculated random numbers */
/* Each number has uniform distribution over range [-1.0, +1.0] */
static double rnd[PERLIN_YSIZE+1][PERLIN_XSIZE+1];

/* Hash implemented as a lookup table; all random numbers are pre-calculated */
/* Deliberately using a real PRNG to keep any statistical analysis clean */
static double hash(int x, int y)
{
	return rnd[y][x];
}

/* Function to interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
double interpolate(double a0, double a1, double w)
{
    /* // You may want clamping by inserting:
     * if (0.0 > w) return a0;
     * if (1.0 < w) return a1;
     */
    /* return (a1 - a0) * w + a0;
     * return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
     * return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
     */
    return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}

typedef struct {
    double x, y;
} vector2;

/* Create random direction vector
 */
vector2 randomGradient(int ix, int iy)
{
    /*
    // Random double. No precomputed gradients mean this works for any number of grid coordinates
    double random = 2920.0 * sin(ix * 21942.0 + iy * 171324.0 + 8912.0) * cos(ix * 23157.0 * iy * 217832.0 + 9758.0);
    */
    double random = hash(ix, iy);
    return (vector2) { .x = cos(random), .y = sin(random) };
}

// Computes the dot product of the distance and gradient vectors.
double dotGridGradient(int ix, int iy, double x, double y)
{
    // Get gradient from integer coordinates
    vector2 gradient = randomGradient(ix, iy);

    // Compute the distance vector
    double dx = x - (double)ix;
    double dy = y - (double)iy;

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y);
}

// Compute Perlin noise at coordinates x, y
static double perlin(double x, double y)
{
    // Determine grid cell coordinates
    int x0 = (int)x;
    int x1 = x0 + 1;
    int y0 = (int)y;
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    double sx = x - (double)x0;
    double sy = y - (double)y0;

    // Interpolate between grid point gradients
    double n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value;
}

/* Initialize PRNG */
/* NOTE: please call srand() before calling fpn_init() */
void perlin_init(void)
{
	int x, y;

	/* Pre-calculate all necessary random numbers */
	for (y = 0; y < PERLIN_YSIZE+1; y++)
	{
		for (x = 0; x < PERLIN_XSIZE+1; x++)
		{
			/* Using rand(), despite considered harmful.
			 * I suspect it will be OK in gcc.
			 */
			rnd[y][x] = 2.0 * 3.14159265358979323846 * rand() / RAND_MAX;
		}
	}
}

/* Calculate noise for specific unit coordinates */
double perlin_noise(double x, double y)
{
	return x >= 0 && x < PERLIN_XSIZE && y >= 0 && y < PERLIN_YSIZE
		? perlin(x, y)
		: 0;
}
