# fir-cpp
fir-cpp is a small C++ library for FIR calculations. Currently it has:
- firls: least squares design method for type I and type II symmetric FIR filters
- firfreqz: fast frequency response calculation (magnitude only) of FIR filters using FFT

The firls implementation is a translation of SciPy signal.firls from Python to C++, and extended for type II FIR filters. Many of the comments in the source code are copied verbatim from this version.

# build instructions
fir-cpp uses CMake.
```
cmake -B build
cmake --build build
ctest --test-dir build
```
See also the folder speed/ for some speed tests.

# JavaScript/Emscripten
See the folder javascript/ for simple build scripts + demos in JavaScript.

# references
- [SciPy signal.firls](https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.firls.html) - Python implementation for type I FIR filters, used as basis for this implementation. 
- [unofficial Octave firls by Ionescu Vlad](https://savannah.gnu.org/bugs/?func=detailitem&item_id=51310) - Octave implementation for type I-IV FIR filters, used for validation of this implementation. Not (yet) part of Octave.
- [Eigen](https://eigen.tuxfamily.org/) - C++ template library for matrix manipulations. fir-cpp uses the complete orthogonal decomposition to solve the equations. Compile time dependency.
- [KISS FFT by Mark Borgerding](https://github.com/mborgerding/kissfft) - C/C++ library for FFT calculation, used for calculating efficiently the frequency response. Some source files from release 131.1.0 have been copied in this project. See the folder kissfft.

SciPy signal.firls and Octave firls both refer to the following article for a description of the algorithm:
- Ivan Selesnick, Linear-Phase Fir Filter Design By Least Squares. OpenStax CNX. Aug 9, 2005
This article seems to be no longer available.
