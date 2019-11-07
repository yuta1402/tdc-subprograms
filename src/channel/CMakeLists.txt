cmake_minimum_required(VERSION 3.1)

set(target channel)

set(sources
    ./tdc.cpp
)

add_library(${target} ${sources})
