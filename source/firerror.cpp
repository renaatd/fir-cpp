#include "fir.hpp"

static const char *fir_error_messages[] = {
    "OK",
    "Number of taps must be positive!",
    "Frequency must be positive!",
    "Number of frequency bands must be positive!",
    "Frequency bands must be monotonic array with positive width!",
    "Weights must be positive!",
    "Invalid error code!"};

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))

const char *firerror(int errnum) {
    const int no_elements = (int)ARRAY_LENGTH(fir_error_messages);
    if (errnum < 0 || errnum >= no_elements) {
        return fir_error_messages[no_elements - 1];
    }
    return fir_error_messages[errnum];
}
