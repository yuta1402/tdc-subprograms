cmake_minimum_required(VERSION 3.1)

set(target hello_test)

add_executable(${target}
    ./main.cpp
)

target_link_libraries(${target}
    hs
)

add_test(
    NAME hello
    COMMAND $<TARGET_FILE:${target}>
)
