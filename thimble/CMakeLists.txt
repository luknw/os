cmake_minimum_required(VERSION 3.7)
project(thimble)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")

set(SOURCE_FILES thimble.c liblogger/logger.c libsafe/safeExit.c libsafe/safeAlloc.c libsafe/safeSignal.c)
add_executable(needles ${SOURCE_FILES})
target_link_libraries(needles pthread)