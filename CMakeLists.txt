cmake_minimum_required(VERSION 3.20)

project(jlang)

set(CMAKE_CXX_STANDARD 23)
set(WARNING_FLAGS "-Werror" "-Wall" "-Wextra" "-Wno-implicit-fallthrough" "-Wno-unused-const-variable")
set(DEBUG_FLAGS ${WARNING_FLAGS} "-O0" "-ggdb" )

add_subdirectory(frozen)
add_subdirectory(jas)
add_subdirectory(elf)
