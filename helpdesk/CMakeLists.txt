cmake_minimum_required(VERSION 3.7)
project(helpdesk)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")


add_executable(clerk
        clerk.c
        liblogger/logger.c
        libsafe/safeSignal.c
        libsafe/safeIO.c
        libhashmap/hashMap.c
        liblinkedlist/linkedList.c
        libarrayqueue/arrayQueue.c
        libsafe/safeAlloc.c
        libsafe/safeExit.c
        protocol.h
        protocol.c)
target_link_libraries(clerk pthread)

add_executable(worker
        worker.c
        protocol.h
        protocol.c
        liblogger/logger.c
        libsafe/safeIO.c)
target_link_libraries(worker pthread)
