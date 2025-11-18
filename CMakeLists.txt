cmake_minimum_required(VERSION 3.22)
project(shrink.cpp VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-pthread -fwrapv -Wall)
# add_link_options(-fuse-ld=mold)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    add_compile_options(-fconcepts-diagnostics-depth=10)
endif()

add_compile_options(-O0 -g -fsanitize=address)
add_link_options(-fsanitize=address)

add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(test)
