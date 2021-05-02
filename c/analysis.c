/* spectrum.c - spectrum analysis of different noise algorithms in 2D
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 *
 * Generates 2D noise as a greyscale image in PGM format.
 * Usage: amplitude [<algorithm>] [<number of replicates>]
 * Optional: for amplitude analysis, pass replication count as an argument.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defines.h"
#include "output.h"
#include "fpn.h"
#include "fpnsin.h"
#include "perlin.h"
#include "simplex.h"
#ifdef SPECTRUM
#include "fft.h"
#endif

/* seed for PRNG */
#define SEED	1

/* Variance calculation needs to be fed with a mean */
#define EXPECTED_MEAN	0.0

/* Collect statistics for every pixel */
static double mean[YPIXELS][XPIXELS];
static double vari[YPIXELS][XPIXELS];

#ifdef SPECTRUM
static double *buf;
#endif

static double no_noise(double x, double y)
{
	(void)x;
	(void)y;
	return 0.0;
}

static double (*pick_noise(const char *algorithm))(double, double)
{
	if (strcmp(algorithm, "perlin") == 0)
	{
		perlin_init();
		return perlin_noise;
	}
	else if (strcmp(algorithm, "simplex") == 0)
	{
		simplex_init();
		return simplex_noise;
	}
	else if (strcmp(algorithm, "fpn") == 0)
	{
		/* m(x,y) = h1*x+h2*y+h3; less directional artifacts */
		static double weights[] = {1.0, 1.0, 1.0};
		fpn_init(weights);
		return fpn_noise;
	}
	else if (strcmp(algorithm, "fpn0") == 0)
	{
		/* m(x,y) = h1*x+h2*y; more band-limited */
		static double weights[] = {1.0, 1.0, 0.0};
		fpn_init(weights);
		return fpn_noise;
	}
	else if (strcmp(algorithm, "fpnsin") == 0)
	{
		static double weights[] = {1.0, 1.0, 1.0};
		fpnsin_init(weights);
		return fpnsin_noise;
	}
	else
	{
		return no_noise;
	}
}

void replicate(const char *algorithm, int samples)
{
	int x, y;
	double (*noise)(double, double) = pick_noise(algorithm);
#ifdef SPECTRUM
	double *p = buf;
#endif

	/* Generate noise */
	for (y = 0; y < YPIXELS; y++)
	{
		for (x = 0; x < XPIXELS; x++)
		{
			double ux = (double)x * XUNITS / XPIXELS;
			double uy = (double)y * YUNITS / YPIXELS;
			double h = noise(ux, uy);
#ifdef SPECTRUM
			*p++ = h;
		}
	}

	/* Calculate spectrum */
	fft_execute();

	/* Average */
	p = buf;
	for (y = 0; y < YPIXELS; y++)
	{
		for (x = 0; x < XPIXELS; x++)
		{
			mean[y][x] += *p++ / samples;
#else
			/* Calculate mean and variance */
			mean[y][x] += h / samples;
			if (samples > 1)
			{
				double d = h - EXPECTED_MEAN;
				vari[y][x] += d*d / (samples-1);
			}
#endif
		}
	}
}

int main(int argc, char *argv[])
{
	const char *algorithm = argc > 1 ? argv[1] : "fpn";
	int x, y, i, samples = argc > 2 ? atoi(argv[2]) : 1;

	/* Clear arrays */
	for (y = 0; y < YPIXELS; y++)
	{
		for (x = 0; x < XPIXELS; x++)
		{
			mean[y][x] = vari[y][x] = 0.0;
		}
	}
#ifdef SPECTRUM
	buf = fft_init();
#endif

	/* Replicate the noise algorithm multiple times */
	srand(SEED);
	for (i = 0; i < samples; i++)
	{
		fprintf(stderr, "\r%d/%d", i, samples);
		replicate(algorithm, samples);
	}
	fprintf(stderr, "\r%d/%d\n", samples, samples);

#ifdef SPECTRUM
	/* Spectrum */
	outputImage(mean, "%s_%d_fft.pgm", algorithm, samples);
	fft_exit();
#else
	/* Mean and variance */
	outputImage(mean, "%s_%d_mean.pgm", algorithm, samples);
	outputImage(vari, "%s_%d_vari.pgm", algorithm, samples);
#endif
	return 0;
}
