cmake_minimum_required(VERSION 3.1)

set(target hs)

set(sources
    ./hello.cpp
)

add_library(${target} ${sources})
