add_executable(speed_firls
    speed_firls.cpp
)
target_link_libraries(
    speed_firls
    PRIVATE
    fir
)

add_executable(speed_freqz
    speed_freqz.cpp
)
target_link_libraries(
    speed_freqz
    PRIVATE
    fir
    fir_extra
)