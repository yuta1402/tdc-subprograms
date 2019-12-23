cmake_minimum_required(VERSION 3.1)

set(target lcstdc)

set(sources
    ./bp_marker_decoder.cpp
)

add_custom_target(lcstdc_prob_table ALL COMMAND "cp" "-r" "${PROJECT_SOURCE_DIR}/data/prob_table" "${PROJECT_BINARY_DIR}")

add_library(${target} ${sources})

target_link_libraries(${target}
    tdcs
    lcs
)
