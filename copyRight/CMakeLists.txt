cmake_minimum_required(VERSION 3.7)
project(copyRight)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

set(SOURCE_FILES main.cpp Client.cpp Client.h ServerAddress.cpp ServerAddress.h)
add_executable(copyRight ${SOURCE_FILES})

add_executable(server serverExample.cpp)

add_executable(client clientExample.cpp)

find_library(RSYNC_LIBRARY rsync /usr/local/lib/)
target_link_libraries(copyRight ${RSYNC_LIBRARY} pthread)
