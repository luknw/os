cmake_minimum_required(VERSION 3.7)
project(spindle)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")

set(SOURCE_FILES
        spindle.c
        libsafe/safeAlloc.c
        libsafe/safeExit.c
        liblogger/logger.c
        libarrayqueue/arrayQueue.c
        )
add_executable(spindle ${SOURCE_FILES})
target_link_libraries(spindle pthread)