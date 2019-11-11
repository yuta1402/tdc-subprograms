cmake_minimum_required(VERSION 3.1)

set(target pcstdc)

set(sources
    ./sc_decoder.cpp
)

add_library(${target} ${sources})
