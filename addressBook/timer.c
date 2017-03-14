//
// Created by luknw on 3/13/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "timer.h"


static const char *LOG_TIME_FORMAT = "real: %10lfs\tuser: %10lfs\tsystem: %10lfs\t\t%s\n";
static long sysClocksPerSec = NULL;


long getClocksPerSec(void) {
    return (sysClocksPerSec != NULL) ? sysClocksPerSec : (sysClocksPerSec = sysconf(_SC_CLK_TCK));
}


clock_t clock_t_add(clock_t a, clock_t b) {
    return a + b;
}

Seconds Seconds_div(Seconds a, Seconds b) {
    return a / b;
}

ClockInterval ClockInterval_mapFunction(ClockInterval a, ClockInterval b, clock_t (*func)(clock_t, clock_t)) {
    ClockInterval result;

    result.realClocks = func(a.realClocks, b.realClocks);
    result.userClocks = func(a.userClocks, b.userClocks);
    result.systemClocks = func(a.systemClocks, b.systemClocks);

    return result;
}

SecondsInterval SecondsInterval_mapFunctionSeconds(SecondsInterval a, Seconds b, Seconds (*func)(Seconds, Seconds)) {
    SecondsInterval result;

    result.realSeconds = func(a.realSeconds, b);
    result.userSeconds = func(a.userSeconds, b);
    result.systemSeconds = func(a.systemSeconds, b);

    return result;
}


ClockInterval ClockInterval_add(ClockInterval a, ClockInterval b) {
    return ClockInterval_mapFunction(a, b, clock_t_add);
}

SecondsInterval SecondsInterval_divSeconds(SecondsInterval a, Seconds b) {
    return SecondsInterval_mapFunctionSeconds(a, b, Seconds_div);
}

SecondsInterval ClockInterval_toSeconds(ClockInterval interval) {
    SecondsInterval result;

    result.realSeconds = interval.realClocks / (Seconds) getClocksPerSec();
    result.userSeconds = interval.userClocks / (Seconds) getClocksPerSec();
    result.systemSeconds = interval.systemClocks / (Seconds) getClocksPerSec();

    return result;
}


tms tms_diff(tms end, tms start) {
    tms result;

    result.tms_utime = end.tms_utime - start.tms_utime;
    result.tms_stime = end.tms_stime - start.tms_stime;

    result.tms_cutime = end.tms_cutime - start.tms_cutime;
    result.tms_cstime = end.tms_cstime - start.tms_cstime;

    return result;
}

ClockInterval ClockInterval_get(tms start, tms end, clock_t rStart, clock_t rEnd) {
    tms intervals = tms_diff(end, start);

    ClockInterval result;
    result.realClocks = rEnd - rStart;
    result.userClocks = intervals.tms_utime + intervals.tms_cutime;
    result.systemClocks = intervals.tms_stime + intervals.tms_cstime;

    return result;
}

void ClockInterval_print(ClockInterval interval, char *description) {
    SecondsInterval_print(ClockInterval_toSeconds(interval), description);
}

void SecondsInterval_print(SecondsInterval interval, char *description) {
    if (description == NULL) return;

    printf(LOG_TIME_FORMAT,
           interval.realSeconds, interval.userSeconds, interval.systemSeconds,
           description);
}
