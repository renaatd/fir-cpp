#include "fir.hpp"
#include "firfreqz_naive.hpp"
#include "stopwatch_elapsed.h"
#include <cerrno>
#include <climits>
#include <cmath>
#include <stdio.h>

/**
 * Parse string s to integer, return true on success, false on failure. The string can use the 0x
 * prefix
 * @param result: result of conversion, only updated on success
 */
static bool parse_int(int *result, const char *s) {
    char *endptr;
    errno = 0;
    long n = strtol(s, &endptr, 0);
    if (errno != 0 || endptr == s || *endptr != '\0') {
        return false;
    }
    if (n < INT_MIN || n > INT_MAX) {
        return false;
    }
    *result = (int)n;
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "speed_freqz <taps> <N_START> <N_END> <N_STEP>\n");
        exit(EXIT_FAILURE);
    }

    int taps;
    int n_start;
    int n_end;
    int n_step;
    if (!parse_int(&taps, argv[1]) || !parse_int(&n_start, argv[2]) ||
        !parse_int(&n_end, argv[3]) || !parse_int(&n_step, argv[4])) {
        fprintf(stderr, "All parameters must be positive integer!\n");
    }
    if (taps <= 0 || n_start <= 0 || n_end <= 0 || n_step <= 0) {
        fprintf(stderr, "All parameters must be positive integer!\n");
    }

    const int NUMBANDS = 2;
    const FirFloat a = 0.1; // width of the transition band

    FirFloat bands[2 * NUMBANDS] = {0, a, 0.5 - a, 0.5};
    FirFloat desired[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 1};
    FirFloat h[taps];

    FirFloat frequencies[n_end];
    FirFloat magnitudes[n_end];
    FirFloat magnitudes_naive[n_end];

    Stopwatch s;
    firls(h, taps, NUMBANDS, bands, desired, desired, weight, 1.0);
    int elapsed = s.elapsed();
    printf("firls %3d taps: %6d us\n", taps, elapsed);

    for (int i = n_start; i <= n_end; i += n_step) {
        {
            Stopwatch s;
            firfreqz(frequencies, magnitudes, i, taps, h, 2.0);
            int elapsed = s.elapsed();
            printf("freqz fft   %3d: %6d us\n", i, elapsed);
        }

        {
            Stopwatch s;
            firfreqz_naive(frequencies, magnitudes_naive, i, taps, h, 2.0);
            int elapsed = s.elapsed();
            printf("freqz naive %3d: %6d us\n", i, elapsed);
        }

        for (int j = 0; j < i; j++) {
            if (std::abs(magnitudes[j] - magnitudes_naive[j]) > 1e5) {
                fprintf(stderr,
                        "Oops, deviation at %d points, index %d: magnitudes[]: %lf, "
                        "magnitudes_naive[]: %lf\n",
                        i, j, magnitudes[j], magnitudes_naive[j]);
            }
        }
    }
}