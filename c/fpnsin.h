/* fpnsin.h - Fractional Polynomial Noise in 2D, sine carrier
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

extern void fpnsin_init(double weights[3]);
extern double fpnsin_noise(double x, double y);
