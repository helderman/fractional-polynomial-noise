/* fpn.c - reference implementation of Fractional Polynomial Noise in 2D
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

#include <math.h>
#include <stdlib.h>
#include "defines.h"
#include "fpn.h"

#ifndef D
#define D	4
#endif

#ifndef S
#define S	4.67970975809363
#endif

/* Collection of pre-calculated random numbers */
/* Each number has uniform distribution over range [-1.0, +1.0] */
static double rnd[3][YUNITS][XUNITS];

/* Hash implemented as a lookup table; all random numbers are pre-calculated */
/* By using a real PRNG, I hope to keep any statistical analysis clean */
static double hash(int x, int y, int z)
{
	return rnd[z][YTILE(y)][XTILE(x)];
}

/* The fractional polynomial that drives the gradient noise */
static double f(int gx, int gy, double lx, double ly)
{
	return (hash(gx, gy, 0) * lx + hash(gx, gy, 1) * ly + hash(gx, gy, 2)) *
		pow((1.0 - lx*lx) * (1.0 - ly*ly), S);
}

/* For every pixel, there are D*D overlapping polynomials */
static double g(double x, double y)
{
	double total = 0.0;
	int j;
	for (j = 0; j < D; j++)
	{
		double ty = (y + j) / D;
		double fy = floor(ty);
		double ly = 2.0 * (ty - fy) - 1.0;
		int gy = (int)(D * fy - j);
		int i;
		for (i = 0; i < D; i++)
		{
			double tx = (x + i) / D;
			double fx = floor(tx);
			double lx = 2.0 * (tx - fx) - 1.0;
			int gx = (int)(D * fx - i);

			total += f(gx, gy, lx, ly);
		}
	}
	return total;
}

/* Initialize PRNG */
/* NOTE: please call srand() before calling fpn_init() */
void fpn_init(double weights[3])
{
	int x, y, z;

	/* Pre-calculate all necessary random numbers */
	for (z = 0; z < 3; z++)
	{
		for (y = 0; y < YUNITS; y++)
		{
			for (x = 0; x < XUNITS; x++)
			{
				/* Using rand(), despite considered harmful.
				 * Its limited range, short period and poor
				 * quality of low-order bits are no problem
				 * here. Main concern is proper distribution.
				 * I suspect it will be OK in gcc.
				 */
				rnd[z][y][x] =
					weights[z] *
					(2.0 * rand() / RAND_MAX - 1.0);
			}
		}
	}
}

/* Calculate noise for specific unit coordinates */
double fpn_noise(double x, double y)
{
	return g(x, y);
}
