/* noise2D.c - generates gradient noise as a greyscale image in PGM format
 * Ruud Helderman, April 2021
 * MIT License
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define BITS	8	/* bits per pixel, e.g. 8 bits = 256 shades of grey */

#define XPIXELS	1024	/* image width in pixels */
#define YPIXELS	1024	/* image height in pixels */

#define XUNITS	32	/* image width in noise units */
#define YUNITS	32	/* image height in noise units */

#define SEED	1	/* seed for PRNG */

/* Collection of pre-calculated random numbers, all in range [-1.0, +1.0] */
static double rnd[3][YUNITS+3][XUNITS+3];

/* Hash implemented as a lookup table; all random numbers are pre-calculated */
static double hash(int x, int y, int z)
{
	return rnd[z][y+1][x+1];
}

/* The fractional polynomial that drives the gradient noise */
static double polynomial(int i, double x, int j, double y)
{
	return (hash(i, j, 0) * x + hash(i, j, 1) * y + hash(i, j, 2)) *
		pow((1.0-0.25*x*x) * (1.0-0.25*y*y), 4.67970975809363);
}

/* For every pixel, there are 4*4 = 16 overlapping polynomials */
static double noise(double px, double py)
{
	double h = 0.0;
	double ux = px * XUNITS / XPIXELS;	/* convert pixels to units */
	double uy = py * YUNITS / YPIXELS;
	int j, ny = (int)floor(uy);
	for (j = ny - 1; j <= ny + 2; j++)
	{
		int i, nx = (int)floor(ux);
		for (i = nx - 1; i <= nx + 2; i++)
		{
			h += polynomial(i, ux-i, j, uy-j);
		}
	}
	return h;
}

int main()
{
	double hmin = 999.9, hmax = -999.9;
	int x, y, z;

	/* Pre-calculate all necessary random numbers */
	srand(SEED);
	for (z = 0; z < 3; z++)
	{
		for (y = 0; y < YUNITS+3; y++)
		{
			for (x = 0; x < XUNITS+3; x++)
			{
				/* Using rand(), although considered harmful.
				 * Its limited range, short period and poor
				 * quality of low-order bits are no problem
				 * here. My main concern is distribution.
				 * I suspect it will be OK in gcc.
				 */
				rnd[z][y][x] = 2.0 * rand() / RAND_MAX - 1.0;
			}
		}
	}

	/* Calculate contrast */
	for (y = 0; y < YPIXELS; y++)
	{
		for (x = 0; x < XPIXELS; x++)
		{
			double h = noise(x, y);	/* x and y cast to double */
			if (h > hmax) hmax = h;
			if (h < hmin) hmin = h;
		}
	}
	fprintf(stderr, "Min = %lf, max = %lf\n", hmin, hmax);

	/* Write image data to stdout */
#ifdef _MSC_VER
	setmode(fileno(stdout), O_BINARY);	/* prevent LF --> CR+LF */
#endif
	printf("P5\n%d %d\n%d\n", XPIXELS, YPIXELS, (1<<BITS)-1);
	for (y = 0; y < YPIXELS; y++)
	{
		for (x = 0; x < XPIXELS; x++)
		{
			double h = noise(x, y);
			double b = 0.999 * (h-hmin) / (hmax-hmin) * (1<<BITS);
			putchar((int)b);	/* b is in range [0, 256) */
		}
	}
	return 0;
}
