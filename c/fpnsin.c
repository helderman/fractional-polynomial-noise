/* fpnsin.c - Fractional Polynomial Noise in 2D, sine carrier
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

#include <math.h>
#include <stdlib.h>
#include "fpnsin.h"

#ifndef D
#define D	4
#endif

#define HALF_PI	(3.14159265358979323846 / 2.0)

/* Collection of pre-calculated random numbers */
/* Each number has uniform distribution over range [-1.0, +1.0] */
static double rnd[3][FPNSIN_YSIZE+D-1][FPNSIN_XSIZE+D-1];

/* Hash implemented as a lookup table; all random numbers are pre-calculated */
/* Deliberately using a real PRNG to keep any statistical analysis clean */
static double hash(int x, int y, int z)
{
	return rnd[z][y+D-1][x+D-1];
}

/* Cosine replacing the fractional polynomial that drives the gradient noise */
static double f(int gx, int gy, double lx, double ly)
{
	return (hash(gx, gy, 0) * lx + hash(gx, gy, 1) * ly + hash(gx, gy, 2)) *
		pow(cos(HALF_PI * lx) * cos(HALF_PI * ly), D);
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
/* NOTE: please call srand() before calling fpnsin_init() */
void fpnsin_init(double weights[3])
{
	int x, y, z;

	/* Pre-calculate all necessary random numbers */
	for (z = 0; z < 3; z++)
	{
		for (y = 0; y < FPNSIN_YSIZE+D-1; y++)
		{
			for (x = 0; x < FPNSIN_XSIZE+D-1; x++)
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
double fpnsin_noise(double x, double y)
{
	return x >= 0 && x < FPNSIN_XSIZE && y >= 0 && y < FPNSIN_YSIZE
		? g(x, y)
		: 0;
}
