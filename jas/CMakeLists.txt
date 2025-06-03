add_executable(jasvm jasvm.cpp)
target_compile_options(jasvm PRIVATE "-O0" "-ggdb" "-Werror" "-Wall" "-Wextra" "-Wno-implicit-fallthrough" "-Wno-unused-const-variable")
target_link_libraries(jasvm frozen::frozen)
