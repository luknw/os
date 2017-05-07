cmake_minimum_required(VERSION 3.7)
project(barberqueue)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic")

set(COMMON_SOURCES
        barbershop.h
        waitingRoomQueue.c
        libconcurrent/semaphore.c libconcurrent/semaphore.h
        libconcurrent/utils.c libconcurrent/utils.h
        libconcurrent/lock.c libconcurrent/lock.h
        libconcurrent/condition.c libconcurrent/condition.h
        libconcurrent/concurrent.h
        libsafe/safeSignal.c libsafe/safeSignal.h
        libsafe/safeIpc.c libsafe/safeIpc.h
        libsafe/safeExit.c libsafe/safeExit.h
        liblogger/logger.c liblogger/logger.h
        libsafe/safeIO.c)

add_executable(barber barber.c
        barbershop.h waitingRoomQueue.c
        ${COMMON_SOURCES})
target_link_libraries(barber pthread)

add_executable(mothership mothership.c
        libsafe/safeFork.c libsafe/safeFork.h
        ${COMMON_SOURCES})
target_link_libraries(mothership pthread)

