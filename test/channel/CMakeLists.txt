cmake_minimum_required(VERSION 3.1)

set(target channel_test)

add_executable(${target}
    ./main.cpp
)

target_link_libraries(${target}
    channel
)

add_test(
    NAME channel
    COMMAND $<TARGET_FILE:${target}>
)
