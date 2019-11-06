cmake_minimum_required(VERSION 3.1)
enable_language(CXX)

project(cpp-project-template)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-stdlib=libc++ -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

set(target a.out)
set(source_dir ./src)

include_directories("./include")

add_subdirectory(${source_dir}/hs)

add_executable(${target} ${source_dir}/main.cpp)

target_link_libraries(${target}
    ${CMAKE_THREAD_LIBS_INIT}
    hs
)

enable_testing()
add_subdirectory("./test")
