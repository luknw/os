//
// Created by luknw on 3/13/17.
//

#ifndef ADDRESSBOOK_TIMER_H
#define ADDRESSBOOK_TIMER_H

#include <time.h>
#include <sys/resource.h>


#define INIT_MEASURE_TIME() \
            timespec _rStart, _rEnd; \
            rusage _start, _end; \
            TimingInfo _timingInfo;

#define MEASURE_TIME(description, actions) \
            if(getrusage(RUSAGE_SELF, &_start) == -1) { \
                perror("Error measuring time: "); \
            } \
            if(clock_gettime(CLOCK_MONOTONIC_RAW, &_rStart) == -1) { \
                perror("Error measuring time: "); \
            } \
            actions \
            if(clock_gettime(CLOCK_MONOTONIC_RAW, &_rEnd) == -1) { \
                perror("Error measuring time: "); \
            } \
            if(getrusage(RUSAGE_SELF, &_end) == -1) { \
                perror("Error measuring time: "); \
            } \
            _timingInfo = TimingInfo_fromInterval(_rStart, _rEnd, _start, _end); \
            TimingInfo_print(description, _timingInfo);


typedef struct timeval timeval;
typedef struct timespec timespec;
typedef struct rusage rusage;

typedef struct TimingInfo TimingInfo;


struct TimingInfo {
    timeval real;
    timeval user;
    timeval system;
};


timeval timespec_toTimeVal(timespec value);

timeval timeval_add(timeval a, timeval b);

timeval timeval_sub(timeval a, timeval b);

TimingInfo TimingInfo_new();

TimingInfo TimingInfo_add(TimingInfo a, TimingInfo b);

TimingInfo TimingInfo_divLong(TimingInfo a, long b);

TimingInfo TimingInfo_fromInterval(timespec rStart, timespec rEnd, rusage start, rusage end);

void TimingInfo_print(char *description, TimingInfo info);


#endif //ADDRESSBOOK_TIMER_H
