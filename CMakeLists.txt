cmake_minimum_required(VERSION 3.5)
project(icfpc2018)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER clang++-3.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
enable_testing()


set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O2 -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG -march=native")

file(GLOB AI_MAINS AI/*/*.cpp)

# AIのMAINのビルド
foreach (AI_MAIN ${AI_MAINS})
    get_filename_component(AI_MAIN_NAME ${AI_MAIN} NAME_WE)
    add_executable(${AI_MAIN_NAME} ${AI_MAIN})
endforeach (AI_MAIN)

# tracerのビルド
add_executable(tracer4 "tracer/tracer4.cpp")
