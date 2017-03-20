cmake_minimum_required(VERSION 3.6)
project(ioBenchmark)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")

set(SOURCE_FILES ioBenchmark.c ioBenchmark.h
        libsafefile/safeFile.h libsafefile/safeFile.c
        libsafealloc/safeAlloc.h libsafealloc/safeAlloc.c)
add_executable(ioBenchmark ${SOURCE_FILES})