cmake_minimum_required(VERSION 3.1)

set(target utl_test)

add_executable(${target}
    ./main.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(${target}
    ${CMAKE_THREAD_LIBS_INIT}
    channel
)

add_test(
    NAME utl
    COMMAND $<TARGET_FILE:${target}>
)
