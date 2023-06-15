#ifndef FIRFREQZ_NAIVE_HPP
#define FIRFREQZ_NAIVE_HPP

#include "fir.hpp"

/**
 * FIR frequency response (magnitude) calculation over full frequency range
 * using naive method.
 *
 * @param frequencies Output frequencies, in range 0 .. fs/2
 * @param magnitudes Output magnitudes for the corresponding frequency
 * @param n     No of values in fresp / mag
 * @param fs    Sample frequency (Hz), used for scaling fresp
 * @param numTamps The number of taps in the filter
 * @param taps  Array with taps
 * @returns 0 on success, -1 on failure
 */
extern "C" int firfreqz_naive(FirFloat frequencies[], FirFloat magnitudes[], int n, int numTaps,
                              const FirFloat taps[], FirFloat fs);
#endif