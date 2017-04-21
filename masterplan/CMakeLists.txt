cmake_minimum_required(VERSION 3.7)
project(masterplan)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")

add_library(safe libsafe/safe.h libsafe/safeAlloc.c libsafe/safeIO.c)

add_executable(oasis oasis.c config.h)
target_link_libraries(oasis safe)

add_executable(plan plan.c)
target_link_libraries(plan safe)
target_link_libraries(plan m)
