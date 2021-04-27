/* defines.h - size of noise (feel free to override in makefile)
 * Ruud Helderman, April 2021
 * MIT License
 * https://github.com/helderman/fractional-polynomial-noise
 */

/* Image width in pixels */
#ifndef XPIXELS
#ifdef PIXELS
#define XPIXELS PIXELS
#else
#define XPIXELS 1024
#endif
#endif

/* Image height in pixels */
#ifndef YPIXELS
#ifdef PIXELS
#define YPIXELS PIXELS
#else
#define YPIXELS 1024
#endif
#endif

/* Image width in noise units */
#ifndef XUNITS
#ifdef UNITS
#define XUNITS UNITS
#else
#define XUNITS 32
#endif
#endif

/* Image height in noise units */
#ifndef YUNITS
#ifdef UNITS
#define YUNITS UNITS
#else
#define YUNITS 32
#endif
#endif
