/* perlin.h - Classical Perlin Noise in 2D
 */

#define PERLIN_XSIZE	16
#define PERLIN_YSIZE	16

extern void perlin_init(void);
extern double perlin_noise(double x, double y);
