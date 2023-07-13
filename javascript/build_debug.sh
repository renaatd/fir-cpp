#!/bin/sh
# Build a javascript debug version, with address sanitizer, readable stack trace
# and allow memory growth. The latter is needed because of address sanitizer.
#
# emsdk must be configured and in the path
# eigen3 must be findable via pkgconfig
set -e
. "$(dirname -- "$0")/settings.sh"

mkdir -p "${OUTPUT_FOLDER}"
cd "${SCRIPT_FOLDER}" || exit 1
echo "Compiling debug version..."
em++ -Wall -Wextra -fexceptions -fsanitize=address -g2 \
  --std=c++17 \
  -Dkiss_fft_scalar=double -ffast-math -fomit-frame-pointer \
  $(pkg-config --cflags eigen3) -I../include -I../kissfft/include \
  -s EXPORT_ES6 -s MODULARIZE -s ALLOW_MEMORY_GROWTH=1 \
  -s STACK_SIZE=200000 \
  -s EXPORTED_FUNCTIONS=_firerror,_firls,_firfreqz,_malloc,_free,_leak_check,_stack_get_free,_getrlimit \
  -s EXPORTED_RUNTIME_METHODS=cwrap \
  -o "${OUTPUT_FOLDER}/fir.mjs" \
  ../source/firls.cpp ../source/firerror.cpp ../source/firfreqz.cpp \
  ../kissfft/source/kiss_fft.cpp ../kissfft/source/kiss_fftr.cpp \
  ./sanitizer.cpp
echo "Done"
