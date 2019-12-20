cmake_minimum_required(VERSION 3.1)

set(target tdcs_test)

add_executable(${target}
    ./main.cpp
)

target_link_libraries(${target}
    tdcs
    channel
)

add_custom_target(tdcs_test_prob_table ALL COMMAND "cp" "-r" "${PROJECT_SOURCE_DIR}/data/prob_table" "${CMAKE_CURRENT_BINARY_DIR}")

add_test(
    NAME tdcs
    COMMAND $<TARGET_FILE:${target}>
)
