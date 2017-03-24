cmake_minimum_required(VERSION 3.6)
project(scripe)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")

set(SOURCE_FILES main.c)
add_executable(scripe ${SOURCE_FILES})