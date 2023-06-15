#include "firfreqz_naive.hpp"
#include "fir.hpp"
#include <complex>

#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

int firfreqz_naive(FirFloat frequencies[], FirFloat magnitudes[], int n, int numTaps,
                   const FirFloat taps[], FirFloat fs) {
    using C_t = std::complex<FirFloat>;
    if (n < 1 || numTaps <= 0 || fs <= 0.0) {
        return -1;
    }
    FirFloat frequencyDelta = (n > 1) ? fs / (2.0 * (n - 1)) : 0.0;
    FirFloat scaledFrequencyDelta = (n > 1) ? M_PI / (n - 1) : 0.0;
    for (int i = 0; i < n; i++) {
        frequencies[i] = i * frequencyDelta;

        C_t sum{};
        FirFloat scaledFrequency = i * scaledFrequencyDelta;
        for (int j = 0; j < numTaps; j++) {
            sum += taps[j] * std::exp(C_t(0, scaledFrequency * j));
        }
        magnitudes[i] = std::abs(sum);
    }
    return 0;
}