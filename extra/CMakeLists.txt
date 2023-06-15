cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    fir_extra
    source/firfreqz_naive.cpp
)
target_include_directories(
    fir_extra
    PUBLIC
    include/
)
target_compile_options(fir_extra 
    PRIVATE
    -Wall
    -Werror
    -Wconversion
)
target_link_libraries(
    fir_extra
    PRIVATE
    fir
    m
)
