#include "fir.hpp"
#include "stopwatch_elapsed.h"
#include <stdio.h>

int main() {
    const int MAXTAPS = 1010;
    const int NUMBANDS = 2;
    const FirFloat a = 0.1; // width of the transition band

    FirFloat bands[2 * NUMBANDS] = {0, a, 0.5 - a, 0.5};
    FirFloat desired[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 1};
    FirFloat h[MAXTAPS];

    for (int i = 1; i < MAXTAPS; i += 100) {
        Stopwatch s;
        firls(h, i, NUMBANDS, bands, desired, desired, weight, 1.0);
        int elapsed = s.elapsed();
        printf("%3d: %6d us\n", i, elapsed);
    }
}