cmake_minimum_required(VERSION 3.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

find_package(Threads REQUIRED)

set(target tdc-bp-marker-ber-simulator.out)
set(source_dir ./src)

include_directories("./include")

add_executable(${target}
    ${source_dir}/main.cpp
)

target_link_libraries(${target}
    ${CMAKE_THREAD_LIBS_INIT}
    channel
    lcs
    lcstdc
)
