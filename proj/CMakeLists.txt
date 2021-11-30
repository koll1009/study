cmake_minimum_required(VERSION 2.8)

set(PRJ Test)
project(${PRJ})

add_definitions(-g -mssse3 -std=c++11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(value_and_reference)