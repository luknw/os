cmake_minimum_required(VERSION 3.6)
project(flockLock)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")

set(SOURCE_FILES main.c main.h)
add_executable(flockLock ${SOURCE_FILES})