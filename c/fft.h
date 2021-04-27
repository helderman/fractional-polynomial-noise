/* fft.h - fast fourier transform
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

extern double *fft_init(void);
extern void fft_exit(void);
extern void fft_execute(void);
