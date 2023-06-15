# fir-cpp
fir-cpp is a small C++ library for FIR calculations. Currently it has:
- firls: least squares method for type I and type II symmetric FIR filters
- firfreqz: frequency response calculation (magnitude only) of FIR filters

# build instructions
This library uses CMake.
```
cmake -B build
cmake --build build
```

# references
- [SciPy firls](https://docs.scipy.org/doc/scipy/reference/generated/scipy.signal.firls.html) - Python implementation for type I FIR filters, used as basis for this implementation.
- [unofficial Octave firls by Ionescu Vlad](https://savannah.gnu.org/bugs/?func=detailitem&item_id=51310) - Octave implementation for type I-IV FIR filters, used for validation of this implementation. Proposal, not (yet) part of Octave.
- [Eigen](https://eigen.tuxfamily.org/) - C++ template library for matrix manipulations. fir-cpp uses the complete orthogonal decomposition to solve a set of equations.
- [KISS FFT by Mark Borgerding](https://github.com/mborgerding/kissfft) - C/C++ library for FFT calculation, used for calculating efficiently the frequency response.
