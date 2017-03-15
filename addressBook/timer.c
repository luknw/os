//
// Created by luknw on 3/13/17.
//

#include <stdio.h>
#include <stdlib.h>

#include "timer.h"


static const char *TIMING_INFO_FORMAT = "real: %3ld.%06lds\tuser: %3ld.%06lds\tsystem: %3ld.%06lds\t\t%s\n";


timeval timespec_toTimeVal(timespec value) {
    timeval result;
    result.tv_sec = value.tv_sec;
    result.tv_usec = value.tv_nsec / 1000;
    return result;
}

timeval timeval_add(timeval a, timeval b) {
    timeval result;

    __suseconds_t usSum = a.tv_usec + b.tv_usec;

    result.tv_sec = (a.tv_sec + b.tv_sec) + (usSum >= 1000000 ? 1 : 0);
    result.tv_usec = (usSum >= 1000000) ? (1000000 - usSum) : usSum;

    return result;
}

timeval timeval_sub(timeval a, timeval b) {
    timeval result;

    __suseconds_t usDiff = a.tv_usec - b.tv_usec;

    result.tv_sec = (a.tv_sec - b.tv_sec) - (usDiff >= 0 ? 0 : 1);
    result.tv_usec = (usDiff >= 0) ? usDiff : (1000000 + usDiff);

    return result;
}

timeval timeval_divLong(timeval a, long b) {
    timeval result;

    result.tv_sec = a.tv_sec / b;
    result.tv_usec = a.tv_usec / b;

    return result;
}

TimingInfo TimingInfo_new() {
    timeval zero;
    zero.tv_sec = zero.tv_usec = 0;

    TimingInfo result;
    result.real = result.user = result.system = zero;

    return result;
}

TimingInfo TimingInfo_add(TimingInfo a, TimingInfo b) {
    TimingInfo result;

    result.real = timeval_add(a.real, b.real);
    result.user = timeval_add(a.user, b.user);
    result.system = timeval_add(a.system, b.system);

    return result;
}

TimingInfo TimingInfo_divLong(TimingInfo a, long b) {
    TimingInfo result;

    result.real = timeval_divLong(a.real, b);
    result.user = timeval_divLong(a.user, b);
    result.system = timeval_divLong(a.system, b);

    return result;
}

TimingInfo TimingInfo_fromInterval(timespec rStart, timespec rEnd, rusage start, rusage end) {
    TimingInfo result;

    result.real = timeval_sub(timespec_toTimeVal(rEnd), timespec_toTimeVal(rStart));
    result.user = timeval_sub(end.ru_utime, start.ru_utime);
    result.system = timeval_sub(end.ru_stime, start.ru_stime);

    return result;
}

void TimingInfo_print(char *description, TimingInfo info) {
    if (description == NULL) return;

    printf(TIMING_INFO_FORMAT,
           info.real.tv_sec, info.real.tv_usec,
           info.user.tv_sec, info.user.tv_usec,
           info.system.tv_sec, info.system.tv_usec,
           description);

    if (ferror(stdout)) {
        fprintf(stderr, "Error writing to stdout");
        clearerr(stdout);
    }
}
