cmake_minimum_required(VERSION 3.1)

set(target pcstdc_test)

add_executable(${target}
    ./main.cpp
)

target_link_libraries(${target}
    pcstdc
    channel
)

add_custom_target(test_prob_table ALL COMMAND "cp" "-r" "${PROJECT_SOURCE_DIR}/data/prob_table" "${CMAKE_CURRENT_BINARY_DIR}")

add_test(
    NAME pcstdc
    COMMAND $<TARGET_FILE:${target}>
)
