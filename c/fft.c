/* fft.c - fast Fourier transform (wrapper around FFTW)
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

#include <math.h>
#include <fftw3.h>
#include "defines.h"

#define XIN	XPIXELS
#define YIN	YPIXELS
#define XOUT	(XPIXELS/2+1)
#define YOUT	YPIXELS

static double *in;
static fftw_complex *out;
static fftw_plan p;

static double magnitude(fftw_complex c)
{
	double r = c[0], i = c[1];
	double m2 = r*r + i*i;
	return m2;
	//return sqrt(m2);
	//return log(100000.0 + m2);
}

double *fft_init(void)
{
	in = (double *)fftw_malloc(sizeof(double) * XIN * YIN);
	out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * XOUT * YOUT);
	p = fftw_plan_dft_r2c_2d(XIN, YIN, in, out, FFTW_ESTIMATE);
	return in;
}

void fft_exit(void)
{
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);
}

void fft_execute(void)
{
	double *inptr = in;
	fftw_complex *outptr = out;
	int x, y;

	/* Center the FFT output; as suggested in: */
	/* http://www.fftw.org/faq/section3.html#centerorigin */
	for (y = 0; y < YIN; y++)
	{
		for (x = 0; x < XIN; x++)
		{
			*inptr++ *= ((x^y) & 1) == 0 ? 1 : -1;
		}
	}

	/* Fill 'out' with discrete fourier transform of 'in' */
	fftw_execute(p);

	/* Fill 'in' with the magnitude of each complex number in 'out' */
	for (y = 0; y < YOUT; y++)
	{
		in[y*YIN] = magnitude(*outptr++);
		for (x = 1; x < XOUT; x++)
		{
			in[y*YIN+x] = in[y*YIN+XIN-x] = magnitude(*outptr++);
		}
	}
}
