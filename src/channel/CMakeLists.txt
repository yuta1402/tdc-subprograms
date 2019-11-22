cmake_minimum_required(VERSION 3.1)

set(target channel)

set(sources
    ./bsc.cpp
    ./tdc.cpp
)

add_library(${target} ${sources})
