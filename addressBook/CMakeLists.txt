cmake_minimum_required(VERSION 3.6)
project(libaddressbook)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")

add_subdirectory(libaddressbook)

set(SOURCE_FILES benchmark.c timer.c timer.h timer.h)
add_executable(benchmark ${SOURCE_FILES})

target_link_libraries(benchmark addressbook)
target_link_libraries(benchmark ${CMAKE_DL_LIBS})
