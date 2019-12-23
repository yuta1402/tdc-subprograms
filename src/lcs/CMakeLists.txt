cmake_minimum_required(VERSION 3.1)

set(target lcs)

set(sources
    ./bp_decoder.cpp
    ./spmat.cpp
)

add_library(${target} ${sources})
