#include "fir.hpp"
#include "firfreqz_naive.hpp"
#include <gtest/gtest.h>
#include <string.h>

namespace {

TEST(firerror, ok) {
    const char *msg = firerror(0);
    EXPECT_TRUE(strstr(msg, "OK") != NULL);
}

TEST(firerror, bad_args) {
    EXPECT_TRUE(strstr(firerror(-1), "Invalid") != NULL);
    EXPECT_TRUE(strstr(firerror(100), "Invalid") != NULL);
}

TEST(firls, bad_args) {
    const int NUMTAPS = 11;
    const int NUMBANDS = 2;
    FirFloat bands[2 * NUMBANDS] = {};
    FirFloat desired[NUMBANDS] = {};
    FirFloat weight[NUMBANDS] = {};
    FirFloat result[NUMTAPS] = {};

    // numBands zero
    EXPECT_EQ(firls(result, NUMTAPS, 0, bands, desired, desired, weight, 2.0), FIR_ENUMBANDS);
    // non-monotonic bands
    FirFloat bands_non_monotonic1[2 * NUMBANDS] = {0.2, 0.1, 0.3, 0.4};
    EXPECT_EQ(firls(result, NUMTAPS, NUMBANDS, bands_non_monotonic1, desired, desired, weight, 2.0),
              FIR_EBANDS);
    FirFloat bands_non_monotonic2[2 * NUMBANDS] = {0.2, 0.1, 0.3, 0.3};
    EXPECT_EQ(firls(result, NUMTAPS, NUMBANDS, bands_non_monotonic2, desired, desired, weight, 2.0),
              FIR_EBANDS);
    FirFloat bands_non_monotonic3[2 * NUMBANDS] = {0.3, 0.4, 0.1, 0.2};
    EXPECT_EQ(firls(result, NUMTAPS, NUMBANDS, bands_non_monotonic3, desired, desired, weight, 2.0),
              FIR_EBANDS);
    FirFloat bands_non_monotonic4[2 * NUMBANDS] = {0.1, 0.3, 0.2, 0.4};
    EXPECT_EQ(firls(result, NUMTAPS, NUMBANDS, bands_non_monotonic4, desired, desired, weight, 2.0),
              FIR_EBANDS);
    // negative weight
    FirFloat bands_ok[2] = {0.1, 0.2};
    FirFloat desired_ok[2] = {1, 2};
    FirFloat weight_negative[1] = {-1};
    EXPECT_EQ(firls(result, NUMTAPS, 1, bands_ok, desired_ok, desired_ok, weight_negative, 2.0),
              FIR_EWEIGHTS);
}

TEST(firls, firls) {
    const int NUMTAPS = 11;
    const int NUMBANDS = 2;
    const FirFloat a = 0.1; // width of the transition band

    FirFloat bands[2 * NUMBANDS] = {0, a, 0.5 - a, 0.5};
    FirFloat desired[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 1};
    FirFloat h[NUMTAPS];

    // design a halfband symmetric low-pass filter
    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desired, desired, weight, 1.0), 0);
#if 0
    for (int i = 0; i < NUMTAPS; i++) {
        printf("h(%2d): %lf\n", i, h[i]);
    }
#endif

    // make sure it is symmetric
    int mid_x = (NUMTAPS - 1) / 2;
    for (int i = 0; i < mid_x; i++) {
        EXPECT_NEAR(h[i], h[NUMTAPS - 1 - i], 1e-5);
    }
    // make sure the center tap is 0.5
    EXPECT_NEAR(h[mid_x], 0.5, 1e-5);

    // For halfband symmetric, odd coefficients (except the center) should be zero (really small)
    for (int i = 1; i < mid_x; i += 2) {
        EXPECT_NEAR(h[i], 0.0, 1e-5);
    }

    // now check the frequency response
    const int NUMFREQS = 101;
    FirFloat F[NUMFREQS];
    FirFloat H[NUMFREQS];
    EXPECT_EQ(firfreqz(F, H, NUMFREQS, NUMTAPS, h, 1.0), 0);
#if 0
    for (int i = 0; i < NUMFREQS; i++) {
        printf("%lf\n", H[i]);
    }
#endif

    // check that the pass band is close to unity
    for (int i = 0; i < a * NUMFREQS; i++) {
        EXPECT_GT(H[i], 0.999);
    }

    // check that the stop band is close to zero
    for (int i = (1 - a) * NUMFREQS; i < NUMFREQS; i++) {
        EXPECT_LT(H[i], 0.001);
    }
}

TEST(firls, compare_octave) {
    // compare to OCTAVE output
    const int NUMTAPS = 9;
    const int NUMBANDS = 2;

    FirFloat bands[2 * NUMBANDS] = {0, 0.5, 0.55, 1};
    FirFloat desired[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 2};
    FirFloat h[NUMTAPS];

    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desired, desired, weight, 2.0), 0);
    FirFloat knownTaps[NUMTAPS] = {
        -6.26930101730182e-04, -1.03354450635036e-01, -9.81576747564301e-03,
        3.17271686090449e-01,  5.11409425599933e-01,  3.17271686090449e-01,
        -9.81576747564301e-03, -1.03354450635036e-01, -6.26930101730182e-04};
    for (int i = 0; i < NUMTAPS; i++) {
        EXPECT_NEAR(h[i], knownTaps[i], 1e-5);
    }
}

TEST(firls, compare_matlab) {
    // compare to MATLAB output
    const int NUMTAPS = 11;
    const int NUMBANDS = 2;

    FirFloat bands[2 * NUMBANDS] = {0, 0.5, 0.5, 1};
    FirFloat desired[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 2};
    FirFloat h[NUMTAPS];

    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desired, desired, weight, 2.0), 0);
    FirFloat knownTaps[NUMTAPS] = {0.058545300496815,  -0.014233383714318, -0.104688258464392,
                                   0.012403323025279,  0.317930861136062,  0.488047220029700,
                                   0.317930861136062,  0.012403323025279,  -0.104688258464392,
                                   -0.014233383714318, 0.058545300496815};
    for (int i = 0; i < NUMTAPS; i++) {
        EXPECT_NEAR(h[i], knownTaps[i], 1e-5);
    }
}

TEST(firls, compare_octave_type2) {
    // compare to OCTAVE output from Vlad Ionescu's firls (https://savannah.gnu.org/bugs/?func=detailitem&item_id=51310)
    const int NUMTAPS = 6;
    const int NUMBANDS = 2;

    FirFloat bands[2 * NUMBANDS] = {0, 0.1, 0.9, 1};
    FirFloat desired[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 1};
    FirFloat h[NUMTAPS];

    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desired, desired, weight, 2.0), 0);
    FirFloat knownTaps[NUMTAPS] = {-0.048808, 0.080896, 0.467929, 0.467929, 0.080896, -0.048808};
    for (int i = 0; i < NUMTAPS; i++) {
        EXPECT_NEAR(h[i], knownTaps[i], 1e-5);
    }
}

TEST(firls, compare_octave_type2_linear) {
    // compare to OCTAVE output from Vlad Ionescu's firls (https://savannah.gnu.org/bugs/?func=detailitem&item_id=51310)
    const int NUMTAPS = 6;
    const int NUMBANDS = 2;

    FirFloat bands[2 * NUMBANDS] = {0, 0.1, 0.9, 1};
    FirFloat desiredBegin[NUMBANDS] = {0, 1};
    FirFloat desiredEnd[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 1};
    FirFloat h[NUMTAPS];

    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desiredBegin, desiredEnd, weight, 2.0), 0);
    FirFloat knownTaps[NUMTAPS] = {-1.050127, -1.774989, 2.917441, 2.917441, -1.774989, -1.050127};
    for (int i = 0; i < NUMTAPS; i++) {
        EXPECT_NEAR(h[i], knownTaps[i], 1e-5);
    }
}

TEST(firls, compare_octave_type2_linear2) {
    // compare to OCTAVE output from Vlad Ionescu's firls (https://savannah.gnu.org/bugs/?func=detailitem&item_id=51310)
    // Octave: h = firls(5, [0, 0.1, 0.4, 0.6, 0.9, 1.0], [1, 1.5, 2, 2.5, 1, 0], [1,2,0.5], 'n')
    const int NUMTAPS = 6;
    const int NUMBANDS = 3;

    FirFloat bands[2 * NUMBANDS] = {0, 0.1, 0.4, 0.6, 0.9, 1};
    FirFloat desiredBegin[NUMBANDS] = {1, 2, 1};
    FirFloat desiredEnd[NUMBANDS] = {1.5, 2.5, 0};
    FirFloat weight[NUMBANDS] = {1, 2, 0.5};
    FirFloat h[NUMTAPS];

    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desiredBegin, desiredEnd, weight, 2.0), 0);
    FirFloat knownTaps[NUMTAPS] = {0.096153, -0.592161, 1.111733, 1.111733, -0.592161, 0.096153};
    for (int i = 0; i < NUMTAPS; i++) {
        EXPECT_NEAR(h[i], knownTaps[i], 1e-5);
    }
}

TEST(firls, compare_matlab_linear_changes) {
    const int NUMTAPS = 7;
    const int NUMBANDS = 3;

    FirFloat bands[2 * NUMBANDS] = {0, 1, 2, 3, 4, 5};
    FirFloat desiredBegin[NUMBANDS] = {1, 0, 1};
    FirFloat desiredEnd[NUMBANDS] = {0, 1, 0};
    FirFloat weight[NUMBANDS] = {1, 1, 1};
    FirFloat h[NUMTAPS];

    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desiredBegin, desiredEnd, weight, 20.0), 0);
    FirFloat knownTaps[NUMTAPS] = {1.156090832768218,   -4.1385894727395849, 7.5288619164321826,
                                   -8.5530572592947856, 7.5288619164321826,  -4.1385894727395849,
                                   1.156090832768218};
    for (int i = 0; i < NUMTAPS; i++) {
        EXPECT_NEAR(h[i], knownTaps[i], 1e-5);
    }
}

TEST(firls, rank_deficient) {
    const int NUMTAPS = 21;
    const int NUMBANDS = 2;

    FirFloat bands[2 * NUMBANDS] = {0.0, 0.1, 0.9, 1.0};
    FirFloat desired[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 1};
    FirFloat h[NUMTAPS];

    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desired, desired, weight, 2.0), 0);
#if 0
    for (int i = 0; i < NUMTAPS; i++) {
        printf("h(%2d): %lf\n", i, h[i]);
    }
#endif

    // now check the frequency response
    const int NUMFREQS = 512;
    FirFloat F[NUMFREQS];
    FirFloat H[NUMFREQS];
    EXPECT_EQ(firfreqz(F, H, NUMFREQS, NUMTAPS, h, 2.0), 0);
#if 0
    for (int i = 0; i < NUMFREQS; i++) {
        printf("%lf\n", H[i]);
    }
#endif

    // check that the pass band is close to unity
    // check that the stop band is close to zero
    for (int i = 0; i < 2; i++) {
        EXPECT_GT(H[i], 0.99999);
        EXPECT_LT(H[511 - i], 0.00001);
    }

    const int NUMTAPS_LONG = 101;
    FirFloat h_long[NUMTAPS_LONG];
    EXPECT_EQ(firls(h_long, NUMTAPS_LONG, NUMBANDS, bands, desired, desired, weight, 2.0), 0);
    EXPECT_EQ(firfreqz(F, H, NUMFREQS, NUMTAPS_LONG, h_long, 2.0), 0);

    for (int i = 0; i < NUMFREQS; i++) {
        if (F[i] < 0.01) {
            EXPECT_GT(H[i], 0.99999);
        }
        if (F[i] > 0.99) {
            EXPECT_LT(H[i], 0.00001);
        }
    }
}

TEST(firls, rank_deficient2) {
    /* 
     * This testcase initially failed in the transition band due to numerical
     * inaccuracies. After changing the solver from Eigen::ColPivHouseholderQR
     * to Eigen::CompleteOrthogonalDecomposition, the overshoot in the
     * transition band disappeares.
     */
    const int NUMTAPS = 101;
    const int NUMBANDS = 2;

    FirFloat bands[2 * NUMBANDS] = {0.0, 100.0, 400.0, 500.0};
    FirFloat desired[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 1};
    FirFloat h[NUMTAPS];

    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desired, desired, weight, 1000.0), 0);

    // now check the frequency response. Band is DC-500 Hz, and we ask for 501 values -> index = frequency in Hz
    const int NUMFREQS = 501;
    FirFloat F[NUMFREQS];
    FirFloat H[NUMFREQS];
    EXPECT_EQ(firfreqz(F, H, NUMFREQS, NUMTAPS, h, 2.0), 0);

    // Check that all values in transition band are below 1.01
    for (int i = 100; i < 400; i++) {
        EXPECT_LT(H[i], 1.01);
    }
}

TEST(freqz, compare_freqz_naive) {
    // use other test to generate taps
    const int NUMTAPS = 9;
    const int NUMBANDS = 2;

    FirFloat bands[2 * NUMBANDS] = {0, 0.5, 0.55, 1};
    FirFloat desired[NUMBANDS] = {1, 0};
    FirFloat weight[NUMBANDS] = {1, 2};
    FirFloat h[NUMTAPS];

    EXPECT_EQ(firls(h, NUMTAPS, NUMBANDS, bands, desired, desired, weight, 2.0), 0);
    FirFloat knownTaps[NUMTAPS] = {
        -6.26930101730182e-04, -1.03354450635036e-01, -9.81576747564301e-03,
        3.17271686090449e-01,  5.11409425599933e-01,  3.17271686090449e-01,
        -9.81576747564301e-03, -1.03354450635036e-01, -6.26930101730182e-04};
    for (int i = 0; i < NUMTAPS; i++) {
        EXPECT_NEAR(h[i], knownTaps[i], 1e-5);
    }

    // now check the frequency response
    const int NUMFREQS = 64;
    FirFloat F[NUMFREQS];
    FirFloat H[NUMFREQS];
    EXPECT_EQ(firfreqz(F, H, NUMFREQS, NUMTAPS, h, 1.0), 0);
#if 0
    for (int i = 0; i < NUMFREQS; i++) {
        printf("%lf\n", H[i]);
    }
#endif

    FirFloat F2[NUMFREQS];
    FirFloat H2[NUMFREQS];
    EXPECT_EQ(firfreqz_naive(F2, H2, NUMFREQS, NUMTAPS, h, 1.0), 0);

    for (int i = 0; i < NUMFREQS; i++) {
        EXPECT_NEAR(F[i], F2[i], 1e-5);
    }
    for (int i = 0; i < NUMFREQS; i++) {
        EXPECT_NEAR(H[i], H2[i], 1e-5);
    }
}

} // namespace
