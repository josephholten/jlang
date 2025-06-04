add_executable(jasvm jasvm.cpp)
target_compile_options(jasvm PRIVATE
 )
target_link_libraries(jasvm frozen::frozen)
