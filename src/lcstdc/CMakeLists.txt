cmake_minimum_required(VERSION 3.1)

set(target lcstdc)

set(sources
    # ./sc_decoder.cpp
    # ./drift_transition_prob.cpp
    # ./info_table_handler.cpp
    # ./polar_encoder.cpp
    # ./frozen_bit_selector.cpp
)

add_custom_target(prob_table ALL COMMAND "cp" "-r" "${PROJECT_SOURCE_DIR}/data/prob_table" "${PROJECT_BINARY_DIR}")

add_library(${target} ${sources})
