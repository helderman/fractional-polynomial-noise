/* fpnsin.h - Fractional Polynomial Noise in 2D, sine carrier
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

#define FPNSIN_XSIZE	32
#define FPNSIN_YSIZE	32

extern void fpnsin_init(double weights[3]);
extern double fpnsin_noise(double x, double y);
