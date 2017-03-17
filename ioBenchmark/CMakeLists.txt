cmake_minimum_required(VERSION 3.6)
project(ioBenchmark)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES ioBenchmark.c ioBenchmark.h)
add_executable(ioBenchmark ${SOURCE_FILES})