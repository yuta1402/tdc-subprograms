cmake_minimum_required(VERSION 3.1)

set(target tdcs)

set(sources
    ./drift_transition_prob.cpp
    ./likelihood_calculator.cpp
)

add_custom_target(prob_table ALL COMMAND "cp" "-r" "${PROJECT_SOURCE_DIR}/data/prob_table" "${PROJECT_BINARY_DIR}")

add_library(${target} ${sources})
