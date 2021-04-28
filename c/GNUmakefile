# Analysis of different noise algorithms in 2D
# Ruud Helderman, April 2021
# MIT License
# https://github.com/helderman/fractional-polynomial-noise
#
# Makefile for GNU Make
# Needs FFTW3: sudo apt install fftw3-dev

all: amplitude spectrum

C = output.c fpn.c fpnsin.c perlin.c simplex.c analysis.c
H = output.h fpn.h fpnsin.h perlin.h simplex.h defines.h

amplitude: $(C) $(H)
	$(CC) -Wall -Wextra -o$@ $(C) -lm

spectrum: $(C) fft.c $(H) fft.h
	$(CC) -Wall -Wextra -o$@ -DSPECTRUM $(C) fft.c -lm -lfftw3
