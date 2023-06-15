#include "fir.hpp"
#include "kiss_fftr.h"
#include <cmath>

int firfreqz(FirFloat frequencies[], FirFloat magnitudes[], int n, int numTaps,
             const FirFloat taps[], FirFloat fs) {
    if (n < 1 || numTaps <= 0 || fs <= 0.0) {
        return -1;
    }

    /*
     * An FFT on 100 points gives 51 output points, including DC and Nyquist
     * frequency. Calculate fftInputLength to have correct output length.
     */
    const int fftInputLength = 2 * (n - 1);
    if (numTaps >= fftInputLength) {
        return -1;
    }

    FirFloat frequencyDelta = (n > 1) ? fs / (2.0 * (n - 1)) : 0.0;
    for (int i = 0; i < n; i++) {
        frequencies[i] = i * frequencyDelta;
    }

    /*
     * For each output value, we need 2 values in the input vector and 2 in the
     * output vector, so 32 bytes/element. So for an output length of 5000, we
     * need approx 150 kB. Default stack in Emscripten is 64 kb, so we must
     * either extend the stack or allocate on the heap. For simplicity, we
     * allocate on the stack.
     */
    kiss_fft_scalar in[fftInputLength];
    kiss_fft_cpx out[fftInputLength / 2 + 1];

    /* Configuration: do a forward FFT, and allocate memory */
    kiss_fftr_cfg cfg;
    if ((cfg = kiss_fftr_alloc(fftInputLength, 0 /* is_inverse_fft */, NULL, NULL)) == NULL) {
        return -1;
    }

    /* input is impulse response (FIR taps) followed by zeroes */
    for (int i = 0; i < numTaps; i++) {
        in[i] = taps[i];
    }
    for (int i = numTaps; i < fftInputLength; i++) {
        in[i] = 0.0;
    }
    kiss_fftr(cfg, in, out);
    free(cfg);

    for (int i = 0; i < fftInputLength / 2 + 1; i++) {
        magnitudes[i] = std::sqrt(out[i].r * out[i].r + out[i].i * out[i].i);
    }

    return 0;
}
