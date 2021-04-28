/* fpn.h - reference implementation of Fractional Polynomial Noise in 2D
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

extern void fpn_init(double weights[3]);
extern double fpn_noise(double x, double y);
