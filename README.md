# Fractional Polynomial Noise

Experimental algorithm for gradient noise.

You can find the formula and a detailed explanation
[here](https://helderman.github.io/fractional-polynomial-noise/docs/index.html).

## WebGL1 implementation (April 2021)

See it in action [here](https://helderman.github.io/fractional-polynomial-noise/webgl/noise2D.html).

Please note the underlying PRNG is
[questionable](https://stackoverflow.com/q/12964279).

## WebGL2 implementation (March 2021)

A bit more eye candy here.
Go to [shadertoy.com](https://www.shadertoy.com/)
and copy/paste [this script](shadertoy/noise2D.txt) into a fresh shader.

I would love to share the shader myself on shadertoy.com,
but I'm having trouble verifying my email address there. Sorry.

## C implementation (April 2021)

Build and run [noise2D.c](c/noise2D.c),
redirecting its output to a file with extension .pgm.
Open the file with an application that accepts
[raw PGM format](https://en.wikipedia.org/wiki/Netpbm#File_formats),
for example [GIMP](https://en.wikipedia.org/wiki/GIMP).
