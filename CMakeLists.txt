cmake_minimum_required(VERSION 3.14)
project(firls_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

# to have test binaries from subprojects available in top level
enable_testing()

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>")

add_subdirectory(kissfft)

add_library(
    fir
    source/firerror.cpp
    source/firls.cpp
    source/firfreqz.cpp
)
target_include_directories(
    fir
    PUBLIC
    include/
)
# In release mode: enable libeigen vectorization with -march=native
# In debug mode: both during compiling and linking use -fsanitize=address
target_compile_options(fir 
    PUBLIC
	$<$<AND:${gcc_like_cxx},$<CONFIG:Debug>>:-fsanitize=address>
    PRIVATE
    $<$<AND:${gcc_like_cxx},$<CONFIG:Release>>:-march=native>
    -Wall
    -Werror
    -Wconversion
)
target_link_options(fir 
    PUBLIC
	$<$<AND:${gcc_like_cxx},$<CONFIG:Debug>>:-fsanitize=address>
)
target_link_libraries(
    fir
    PRIVATE
    Eigen3::Eigen
    kissfft
)

add_subdirectory(extra/)
add_subdirectory(speed/)
add_subdirectory(test/)