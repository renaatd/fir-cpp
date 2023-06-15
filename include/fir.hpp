#ifndef FIR_HPP
#define FIR_HPP

using FirFloat = double;

#define FIR_ENUMTAPS   1
#define FIR_EFREQUENCY 2
#define FIR_ENUMBANDS  3
#define FIR_EBANDS     4
#define FIR_EWEIGHTS   5

extern "C" const char *firerror(int errnum);

/**
 * FIR filter design using least-squares error minimization.
 *
 *  Calculate the filter coefficients for the linear-phase finite
 *  impulse response (FIR) filter which has the best approximation
 *  to the desired frequency response described by `bands` and
 *  `desired` in the least squares sense (i.e., the integral of the
 *  weighted mean-squared error within the specified bands is
 *  minimized).
 *
 * @param result Coefficients of the filter, must have room for numTaps values.
 * @param numTaps The number of taps in the FIR filter (filter order + 1).
 * @param numBands Number of frequency bands, half the number of elements in `bands`.
 * @param bands A monotonic nondecreasing sequence containing the band edges in
 *      Hz. All elements must be non-negative and less than or equal to
 *      the Nyquist frequency. The bands are specified as
 *      frequency pairs, thus, its length must be
 *      even.
 * @param desiredBegin
 *      Desired gain at the begin of each band. Length has to be `numBands`
 * @param desiredEnd
 *      Desired gain at the end of each band. Length has to be `numBands`
 * @param weight
 *      A relative weighting to give to each band region when solving
 *      the least squares problem. Length has to be `numBands`
 * @param fs
 *      The sampling frequency of the signal. Each frequency in `bands`
 *      must be between 0 and `fs/2` (inclusive).
 * @returns 0 on success, -1 on failure
 */
extern "C" int firls(FirFloat result[], int numTaps, int numBands, const FirFloat bands[],
                     const FirFloat desiredBegin[], const FirFloat desiredEnd[],
                     const FirFloat weight[], FirFloat fs);

/**
 * FIR frequency response (magnitude) calculation over full frequency range
 * using FFT. Most efficient for n-1 = power of 2, or n having many small
 * factors 2/3/5, e.g. for n = 2001 or 2049 points
 *
 * @param frequencies Output frequencies, in range 0 .. fs/2
 * @param magnitudes Output magnitudes for the corresponding frequency
 * @param n     No of values in fresp / mag
 * @param fs    Sample frequency (Hz), used for scaling fresp
 * @param numTamps The number of taps in the filter
 * @param taps  Array with taps
 * @returns 0 on success, -1 on failure
 */
extern "C" int firfreqz(FirFloat frequencies[], FirFloat magnitudes[], int n, int numTaps,
                        const FirFloat taps[], FirFloat fs);

#endif
