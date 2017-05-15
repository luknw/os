cmake_minimum_required(VERSION 3.7)
project(needles)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")

set(SOURCE_FILES sewer.c sewer.h libsafe/safeExit.c libsafe/safeAlloc.c libsafe/safeIO.c)
add_executable(needles ${SOURCE_FILES})
target_link_libraries(needles pthread)