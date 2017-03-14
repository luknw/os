//
// Created by luknw on 3/13/17.
//

#ifndef ADDRESSBOOK_TIMER_H
#define ADDRESSBOOK_TIMER_H

#include <time.h>
#include <sys/times.h>


#define INIT_MEASURE_TIME() \
            tms _start; \
            tms _end; \
            clock_t _rStart, _rEnd; \
            ClockInterval _clockInterval;

#define MEASURE_TIME(description, actions) \
            _rStart = times(&_start); \
            actions \
            _rEnd = times(&_end); \
            _clockInterval = ClockInterval_get(_start, _end, _rStart, _rEnd); \
            ClockInterval_print(_clockInterval, description);


typedef double Seconds;

typedef struct tms tms;
typedef struct ClockInterval ClockInterval;
typedef struct SecondsInterval SecondsInterval;

struct ClockInterval {
    clock_t realClocks;
    clock_t userClocks;
    clock_t systemClocks;
};

struct SecondsInterval {
    Seconds realSeconds;
    Seconds userSeconds;
    Seconds systemSeconds;
};


long getClocksPerSec(void);


clock_t clock_t_add(clock_t a, clock_t b);

Seconds Seconds_div(Seconds a, Seconds b);

ClockInterval ClockInterval_mapFunction(ClockInterval a, ClockInterval b, clock_t (*func)(clock_t, clock_t));

SecondsInterval SecondsInterval_mapFunctionSeconds(SecondsInterval a, Seconds b, Seconds (*func)(Seconds, Seconds));


ClockInterval ClockInterval_add(ClockInterval a, ClockInterval b);

SecondsInterval SecondsInterval_divSeconds(SecondsInterval a, Seconds b);

SecondsInterval ClockInterval_toSeconds(ClockInterval interval);


tms tms_diff(tms end, tms start);

ClockInterval ClockInterval_get(tms start, tms end, clock_t rStart, clock_t rEnd);

void ClockInterval_print(ClockInterval interval, char *description);

void SecondsInterval_print(SecondsInterval interval, char *description);


#endif //ADDRESSBOOK_TIMER_H
