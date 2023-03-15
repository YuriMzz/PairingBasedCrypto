#ifndef BENCH_UTIL_H
#define BENCH_UTIL_H

// #include "contiki.h"
#include <stdio.h>

long unsigned int BENCH_UTIL_start, BENCH_UTIL_end;

#ifndef BENCH_OUTER
#define BENCH_OUTER 1
#endif

#ifndef BENCH_INNER
#define BENCH_INNER 1

#endif

#define BINIT(LABEL)                                                    \
    LOG_INFO_("TIME: " LABEL "%*c = ", (int)(32 - strlen(LABEL)), ' '); \
    for (int _b = 0; _b < BENCH_OUTER; _b++) {

#define BRUN(FUNCTION)                                                  \
    unsigned int BENCH_UTIL_start = CLOCK()/CLOCK_TICKS_PER_MS;         \
    for (int _c = 0; _c < BENCH_INNER; _c++) {                          \
        FUNCTION;                                                       \
    }                                                                   \
    unsigned int BENCH_UTIL_end = CLOCK()/CLOCK_TICKS_PER_MS;           \
    LOG_INFO_("%d ", (BENCH_UTIL_end - BENCH_UTIL_start) / (BENCH_INNER));

#define BEND \
    }        \
    LOG_INFO_("\n");

// printf("%f\n", BENCH_UTIL_sum * 1000.0 / (CLOCK_SECOND * BENCH_OUTER * BENCH_INNER));

#endif /* BENCH_UTIL_H */