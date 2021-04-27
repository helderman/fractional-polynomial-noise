# Analysis of different noise algorithms in 2D
# Ruud Helderman, April 2021
# MIT License
# https://github.com/helderman/fractional-polynomial-noise
#
# Makefile for GNU Make
# Needs FFTW; see
# http://www.fftw.org/fftw3_doc/Installation-and-Customization.html

all: amplitude spectrum

C = output.c fpn.c fpnsin.c perlin.c simplex.c analysis.c
H = output.h fpn.h fpnsin.h perlin.h simplex.h defines.h

amplitude: $(C) $(H)
	$(CC) -o$@ $(C)

spectrum: $(C) fft.c $(H) fft.h
	$(CC) -o$@ -DSPECTRUM -lfftw3 $(C) fft.c
