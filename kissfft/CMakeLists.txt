cmake_minimum_required(VERSION 3.14)
project(kissfft LANGUAGES CXX)

add_library(
    kissfft
    source/kiss_fft.cpp
    source/kiss_fftr.cpp
)
target_include_directories(
    kissfft
    PUBLIC
    include
)
target_compile_options(kissfft 
    PUBLIC
    -Dkiss_fft_scalar=double
    PRIVATE
    -Wall
    -Wextra
    #-Wconversion
    -fvisibility=hidden
    -ffast-math
    -fomit-frame-pointer
    -Wcast-align
    -Wcast-qual
    -Wshadow
    -Wwrite-strings
)
target_link_libraries(
    kissfft
    PRIVATE
    m
)

