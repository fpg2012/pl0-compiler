cmake_minimum_required(VERSION 3.20)
project(pl0_compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(pl0_compiler main.cpp parser.cpp)

add_definitions(-DWITH_OUTPUT)