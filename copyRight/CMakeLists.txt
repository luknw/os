cmake_minimum_required(VERSION 3.7)
project(copyRight)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

set(SOURCE_FILES main.cpp)
add_executable(copyRight ${SOURCE_FILES})

find_library(RSYNC_LIBRARY rsync /usr/local/lib/)
target_link_libraries(copyRight ${RSYNC_LIBRARY})
