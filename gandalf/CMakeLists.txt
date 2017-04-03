cmake_minimum_required(VERSION 3.7)
project(gandalf)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES
        main.c
        libarrayqueue/arrayQueue.c libarrayqueue/arrayQueue.h
        libsafe/safe.h libsafe/safeAlloc.c libsafe/safeIO.c)
add_executable(gandalf ${SOURCE_FILES})