cmake_minimum_required(VERSION 3.7)
project(icfpc2018)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wendif-labels -Wfloat-equal -Winit-self -Winline -Wlogical-op -Wmissing-include-dirs -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-promo -Wswitch-default -Wswitch-enum -Wunsafe-loop-optimizations -Wvariadic-macros -Wwrite-strings ")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O2 -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -s -DNDEBUG -march=native")

file(GLOB AI_MAINS AI/core/*.cpp)

# AIのMAINのビルド
foreach (AI_MAIN ${AI_MAINS})
    get_filename_component(AI_MAIN_NAME ${AI_MAIN} NAME_WE)
    add_executable(${AI_MAIN_NAME} ${AI_MAIN})
endforeach (AI_MAIN)
