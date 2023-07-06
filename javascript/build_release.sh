#!/bin/sh
# Build a javascript release version
#
# emsdk must be configured and in the path
# eigen3 must be findable via pkgconfig

OUTPUT_FOLDER=../build_javascript

mkdir -p "$OUTPUT_FOLDER"
em++ -Wall -Wextra -fexceptions -O3 --closure 1 -flto \
  --std=c++17 \
  -Dkiss_fft_scalar=double -ffast-math -fomit-frame-pointer \
  $(pkg-config --cflags eigen3) -I../include -I../kissfft/include \
  -s MODULARIZE -s STRICT \
  -s STACK_SIZE=200000 \
  -s EXPORTED_FUNCTIONS=_firerror,_firls,_firfreqz,_malloc,_free,_leak_check,_stack_get_free \
  -s EXPORTED_RUNTIME_METHODS=cwrap \
  -o "${OUTPUT_FOLDER}/fir.js" \
  ../source/firls.cpp ../source/firerror.cpp ../source/firfreqz.cpp \
  ../kissfft/source/kiss_fft.c ../kissfft/source/kiss_fftr.c \
  ./sanitizer.cpp
