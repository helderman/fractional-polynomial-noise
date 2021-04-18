/* fpn.h - reference implementation of Fractional Polynomial Noise in 2D
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

#define FPN_XSIZE	16
#define FPN_YSIZE	16

extern void fpn_init(void);
extern double fpn_noise(double x, double y);
