cmake_minimum_required(VERSION 3.7)
project(centrallyPlannedEconomy)

set(CMAKE_C_STANDARD 11)

set(CENTRAL_COMMITTEE_SOURCES
        centralCommittee.c
        libsafe/safeAlloc.c libsafe/safeIO.c
        liblinkedlist/linkedList.c
        libhashmap/hashMap.c
        executiveOrders.h)
add_executable(centralCommittee ${CENTRAL_COMMITTEE_SOURCES})

set(REGIONAL_COMMITTEE_SOURCES
        libsafe/safeAlloc.c libsafe/safeIO.c
        liblinkedlist/linkedList.c
        libhashmap/hashMap.c
        regionalCommittee.c
        executiveOrders.h)
add_executable(regionalCommittee ${REGIONAL_COMMITTEE_SOURCES})
