cmake_minimum_required(VERSION 3.1)

set(target pcstdc)

set(sources
    ./sc_decoder.cpp
    ./scl_decoder.cpp
    ./info_table_handler.cpp
    ./polar_encoder.cpp
    ./frozen_bit_selector.cpp
)

add_library(${target} ${sources})

target_link_libraries(${target}
    tdcs
)
