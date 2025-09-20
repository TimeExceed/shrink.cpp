cmake_minimum_required(VERSION 3.22)
project(shrink.cpp VERSION 0.1)

add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:--std=c++20> $<$<COMPILE_LANGUAGE:C>:--std=c17>
    -pthread -fwrapv -Wall)
add_link_options(-fuse-ld=mold)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    add_compile_options(-fconcepts-diagnostics-depth=10)
endif()

add_compile_options(-O0 -g -fsanitize=address)
add_link_options(-fsanitize=address)

add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(test)
