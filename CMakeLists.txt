cmake_minimum_required(VERSION 3.31)
project(tinyplot C)

set(CMAKE_C_STANDARD 11)

add_executable(tinyplot main.c)

if (UNIX AND NOT APPLE)
    target_link_libraries(tinyplot m)
endif ()