/* amplitude.c - amplitude analysis of different noise algorithms in 2D
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
#include "fpn.h"
#include "perlin.h"

#define BITS	8	/* bits per pixel, e.g. 8 bits = 256 shades of grey */

#define XPIXELS	128	/* image width in pixels */
#define YPIXELS	128	/* image height in pixels */

#define XUNITS	16	/* image width in noise units */
#define YUNITS	16	/* image height in noise units */

#define EXPECTED_MEAN	0.0

#define SEED	1	/* seed for PRNG */

static char filename[99];

/* Collect statistics for every pixel */
static double mean[YPIXELS][XPIXELS];
static double vari[YPIXELS][XPIXELS];

static double no_noise(double x, double y)
{
	(void)x;
	(void)y;
	return 0.0;
}

void replicate(const char *algorithm, int samples)
{
	int x, y, z;

	double (*noise)(double, double);
	if (strcmp(algorithm, "perlin") == 0)
	{
		perlin_init();
		noise = perlin_noise;
	}
	else if (strcmp(algorithm, "fpn") == 0)
	{
		fpn_init();
		noise = fpn_noise;
	}
	else
	{
		noise = no_noise;
	}

	/* Calculate mean and variance */
	for (y = 0; y < YPIXELS; y++)
	{
		for (x = 0; x < XPIXELS; x++)
		{
			double ux = (double)x * XUNITS / XPIXELS;
			double uy = (double)y * YUNITS / YPIXELS;
			double h = noise(ux, uy);
			mean[y][x] += h / samples;
			if (samples > 1)
			{
				double d = h - EXPECTED_MEAN;
				vari[y][x] += d*d / (samples-1);
			}
		}
	}
}

static void writeImageData(int xsize, int ysize, double data[YPIXELS][XPIXELS])
{
	double hmin = 999.9, hmax = -999.9;
	int x, y;
	FILE *file;

	/* calculate contrast */
	for (y = 0; y < ysize; y++)
	{
		for (x = 0; x < xsize; x++)
		{
			double h = data[y][x];
			if (h > hmax) hmax = h;
			if (h < hmin) hmin = h;
		}
	}
	fprintf(stderr, "%s: minimum = %lf, maximum = %lf, delta = %lf\n",
		filename, hmin, hmax, hmax-hmin);

	/* write to file */
	file = fopen(filename, "wb");
	if (file == NULL)
	{
		perror(filename);
		return;
	}
	fprintf(file, "P5\n%d %d\n%d\n", xsize, ysize, (1<<BITS)-1);
	for (y = 0; y < ysize; y++)
	{
		for (x = 0; x < xsize; x++)
		{
			double h = data[y][x];
			double b = 0.999 * (h-hmin) / (hmax-hmin) * (1<<BITS);
			putc((int)b, file);	/* b is in range [0, 256) */
		}
	}
	fclose(file);
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

	/* Replicate the noise algorithm multiple times */
	srand(SEED);
	for (i = 0; i < samples; i++)
	{
		if (i % 10 == 0) fprintf(stderr, "\r%d/%d", i, samples);
		replicate(algorithm, samples);
	}
	fprintf(stderr, "\r%d/%d\n", samples, samples);

	/* Mean */
	sprintf(filename, "%s_%d_mean.pgm", algorithm, samples);
	writeImageData(XPIXELS, YPIXELS, mean);

	/* Variance */
	sprintf(filename, "%s_%d_vari.pgm", algorithm, samples);
	writeImageData(XPIXELS, YPIXELS, vari);

	return 0;
}
