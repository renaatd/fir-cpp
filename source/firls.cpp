#include "fir.hpp"
#include <Eigen/Core>
#include <Eigen/QR>
#include <cmath>

using Matrix = Eigen::MatrixXd;
using Vector = Eigen::VectorXd;

static constexpr FirFloat PI = 3.141592653589793238462;
static FirFloat sinc(FirFloat x) noexcept { return (x == 0) ? 1.0 : sin(x * PI) / (x * PI); }

int firls(FirFloat result[], int numTaps, int numBands, const FirFloat bands[],
          const FirFloat desiredBegin[], const FirFloat desiredEnd[], const FirFloat weight[],
          FirFloat fs) {
    if (numTaps < 1) {
        return FIR_ENUMTAPS;
    }
    int M = (numTaps - 1) / 2;
    bool isType2 = (numTaps % 2 == 0);

    FirFloat nyq = 0.5 * fs;
    if (nyq <= 0.0) {
        return FIR_EFREQUENCY;
    }

    // Scale the frequency bands relative to nyquist, and check if frequencies are in range 0-1
    if (numBands <= 0) {
        return FIR_ENUMBANDS;
    }
    FirFloat bands_scaled[2 * numBands];
    for (int i = 0; i < 2 * numBands; i++) {
        bands_scaled[i] = bands[i] / nyq;
        if (bands_scaled[i] < 0 || bands_scaled[i] > 1) {
            return FIR_EBANDS;
        }
    }

    // Check if frequency bands are non-zero width, monotonically increasing
    for (int i = 0; i < numBands; i++) {
        if (bands_scaled[2 * i + 1] <= bands_scaled[2 * i]) {
            return FIR_EBANDS;
        }
        if (i > 0 && bands_scaled[2 * i] < bands_scaled[2 * i - 1]) {
            return FIR_EBANDS;
        }
    }

    // SciPy signal.firls rejects negative values of desired. The algorithm does
    // seem to work correctly for negative values, so we don't check.

    // Check if weight is positive
    for (int i = 0; i < numBands; i++) {
        if (weight[i] < 0) {
            return FIR_EWEIGHTS;
        }
    }

    // Set up the linear matrix equation to be solved, Qa = b

    // We can express Q(k,n) = 0.5 Q1(k,n) + 0.5 Q2(k,n)
    // where Q1(k,n)=q(k-n) and Q2(k,n)=q(k+n), i.e. a Toeplitz plus Hankel.

    // We omit the factor of 0.5 above, instead adding it during coefficient
    // calculation.

    // We also omit the 1/π from both Q and b equations, as they cancel
    // during solving.

    // We have that:
    //     q(n) = 1/π ∫W(ω)cos(nω)dω (over 0->π)
    // Using our normalization ω=πf and with a constant weight W over each
    // interval f1->f2 we get:
    //     q(n) = W∫cos(πnf)df (0->1) = Wf sin(πnf)/πnf
    // integrated over each f1->f2 pair (i.e., value at f2 - value at f1).
    FirFloat q[numTaps];
    for (int i = 0; i < numTaps; i++) {
        q[i] = 0.0;
        for (int j = 0; j < numBands; j++) {
            q[i] += (sinc(i * bands_scaled[2 * j + 1]) * bands_scaled[2 * j + 1] -
                     sinc(i * bands_scaled[2 * j]) * bands_scaled[2 * j]) *
                    weight[j];
        }
        // printf("q(%d): %lf\n", i, q[i]);
    }
    // Now we assemble our sum of Toeplitz and Hankel
    // Q1 = toeplitz(q[:M+1])
    // Q2 = hankel(q[:M+1], q[M:])
    // Q = Q1 + Q2
    Matrix Q = Matrix::Zero(M + 1, M + 1);
    for (int i = 0; i <= M; i++) {
        for (int j = 0; j <= M; j++) {
            // Toeplitz
            int t_index = (i >= j) ? (i - j) : (j - i);
            FirFloat t = q[t_index];
            // Hankel
            int h_index = i + j + (isType2 ? 1 : 0);
            FirFloat h = q[h_index];
            Q(i, j) = t + h;
            // printf("Q1(%d,%d): %lf\n", i, j, t);
            // printf("Q2(%d,%d): %lf\n", i, j, h);
            // printf("Q(%d,%d): %lf\n", i, j, Q(i,j));
        }
    }

    // Now for b(n) we have that:
    //     b(n) = 1/π ∫ W(ω)D(ω)cos(nω)dω (over 0->π)
    // Using our normalization ω=πf and with a constant weight W over each
    // interval and a linear term for D(ω) we get (over each f1->f2 interval):
    //     b(n) = W ∫ (mf+c)cos(πnf)df
    //          = W [f(mf+c)sin(πnf)/πnf + mf**2 cos(nπf)/(πnf)**2]
    // integrated over each f1->f2 pair (i.e., value at f2 - value at f1).

    FirFloat m[numBands];
    FirFloat c[numBands];
    // Choose m and c such that we are at the start and end weights
    for (int i = 0; i < numBands; i++) {
        m[i] = (desiredEnd[i] - desiredBegin[i]) / (bands_scaled[2 * i + 1] - bands_scaled[2 * i]);
        c[i] = desiredBegin[i] - bands_scaled[2 * i] * m[i];
    }

    Vector b = Vector::Zero(M + 1);
    FirFloat halfExtra = (isType2 ? 0.5 : 0.0);
    for (int i = 0; i <= M; i++) {
        for (int j = 0; j < numBands; j++) {
            b(i) += (bands_scaled[2 * j + 1] * (m[j] * bands_scaled[2 * j + 1] + c[j]) *
                         sinc((i + halfExtra) * bands_scaled[2 * j + 1]) -
                     bands_scaled[2 * j] * (m[j] * bands_scaled[2 * j] + c[j]) *
                         sinc((i + halfExtra) * bands_scaled[2 * j])) *
                    weight[j];
        }
    }
#if 0
    for (int i = 0; i<= M; i++) {
        printf("before - b(%d): %lf\n", i, b(i));
    }
#endif
    /*
     * Only for type I filter: coefficient 0 is special, would give division by zero with normal
     * formula. Use L'Hopital's rule here for cos(nπf)/(πnf)**2 @ n=0 b[0] -= m * bands * bands / 2.
     */
    if (!isType2) {
        for (int j = 0; j < numBands; j++) {
            b(0) -= 0.5 * m[j] *
                    (bands_scaled[2 * j + 1] * bands_scaled[2 * j + 1] -
                     bands_scaled[2 * j] * bands_scaled[2 * j]) *
                    weight[j];
        }
    }
    /*
     * b[1:] += m * np.cos(n[1:] * np.pi * bands) / (np.pi * n[1:]) ** 2
     * b = np.dot(np.diff(b, axis=2)[:, :, 0], weight)
     */
    int startIndex = isType2 ? 0 : 1;
    for (int i = startIndex; i <= M; i++) {
        for (int j = 0; j < numBands; j++) {
            FirFloat scale = (i + halfExtra) * PI;
            FirFloat scale_squared = scale * scale;
            FirFloat c1 = cos(scale * bands_scaled[2 * j]) / scale_squared;
            FirFloat c2 = cos(scale * bands_scaled[2 * j + 1]) / scale_squared;
            b(i) += m[j] * (c2 - c1) * weight[j];
        }
    }
#if 0
    for (int i = 0; i<= M; i++) {
        printf("after - b(%d): %lf\n", i, b(i));
    }
#endif

    // SciPy firls starts with lapack posv (= LU) and falls back to gelsy (QR with column pivoting)
    // if this fails.

    // Here we do always complete orthogonal decomposition. See the
    // recommendation in https://eigen.tuxfamily.org/dox/group__LeastSquares.html
    // Speed comparison, WebAssembly, 1001 taps: 91 ms for
    // CompleteOrthogonalDecomposition vs 77 ms for ColPivHouseholderQR, with
    // CompleteOrthogonalDecompostion having much improved stability for filters
    // with too many taps.
#if 0
    Eigen::ColPivHouseholderQR<Eigen::Ref<Eigen::MatrixXd>> qr(Q);
    Vector a = qr.solve(b);
#endif
    Eigen::CompleteOrthogonalDecomposition<Eigen::Ref<Eigen::MatrixXd>> od(Q);
    Vector a = od.solve(b);

    // make coefficients symmetric (linear phase)
    if (!isType2) {
        // type I filter - middle coefficient is doubled
        result[M] = 2 * a(0);
        for (int i = 1; i <= M; i++) {
            result[M + i] = result[M - i] = a(i);
        }
    } else {
        // type II filter
        for (int i = 0; i <= M; i++) {
            result[M + i + 1] = result[M - i] = a(i);
        }
    }
#if 0
    for (int i = 0; i < numTaps; i++) {
        printf("result(%d): %lf\n", i, result[i]);
    }
#endif
    return 0;
}
