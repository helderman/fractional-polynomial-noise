# Analysis of different noise algorithms in 2D
# Ruud Helderman, April 2021
# MIT License
# https://github.com/helderman/fractional-polynomial-noise
#
# Makefile for Windows (NMAKE)
# spectrum.exe needs FFTW3; see http://www.fftw.org/install/windows.html

all: amplitude.exe spectrum.exe

C = output.c fpn.c fpnsin.c perlin.c simplex.c analysis.c
H = output.h fpn.h fpnsin.h perlin.h simplex.h defines.h

amplitude.exe: $(C) $(H)
	cl /Fe$@ $(C)

spectrum.exe: $(C) fft.c $(H) fft.h
	cl /Fe$@ /DSPECTRUM /Ifftw3 $(C) fft.c libfftw3-3.lib
