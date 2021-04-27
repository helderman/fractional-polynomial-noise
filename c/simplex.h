/* simplex.h - Simplex Noise in 2D
 */

#define SIMPLEX_XSIZE	16
#define SIMPLEX_YSIZE	16

extern void simplex_init(void);
extern double simplex_noise(double x, double y);
