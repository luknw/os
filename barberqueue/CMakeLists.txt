cmake_minimum_required(VERSION 3.7)
project(barberqueue)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")


add_executable(barber barber.c)
add_executable(mothership mothership.c)
