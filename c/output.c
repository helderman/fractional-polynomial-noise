/* output.c - creates image files
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

#include <stdarg.h>
#include <stdio.h>
#include "defines.h"
#include "output.h"

#define BITS	8	/* bits per pixel, e.g. 8 bits = 256 shades of grey */

/* Generate a greyscale image in PGM format */
void outputImage(double data[YPIXELS][XPIXELS], const char *fmt, ...)
{
	double hmin = 999.9, hmax = -999.9;
	int x, y;
	FILE *file;

	static char filename[99];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(filename, sizeof(filename), fmt, ap);
	va_end(ap);

	/* calculate contrast */
	for (y = 0; y < YPIXELS; y++)
	{
		for (x = 0; x < XPIXELS; x++)
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
	fprintf(file, "P5\n%d %d\n%d\n", XPIXELS, YPIXELS, (1<<BITS)-1);
	for (y = 0; y < YPIXELS; y++)
	{
		for (x = 0; x < XPIXELS; x++)
		{
			double h = data[y][x];
			double b = 0.999 * (h-hmin) / (hmax-hmin) * (1<<BITS);
			putc((int)b, file);	/* b is in range [0, 256) */
		}
	}
	fclose(file);
}
