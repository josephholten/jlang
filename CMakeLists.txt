cmake_minimum_required(VERSION 3.20)

project(jlang)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)

add_subdirectory(frozen)
add_subdirectory(jas)
