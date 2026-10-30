///
/// \file       stopwatch.c
/// \brief      Elapsed-time measurement with a monotonic clock.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/stopwatch.h>

#include <time.h>

void stopwatch_start(stopwatch *const self) { clock_gettime(CLOCK_MONOTONIC, &self->start); }

double stopwatch_elapsed_ms(const stopwatch *const self) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    return timespec_difference_ms(&self->start, &now);
}

double timespec_difference_ms(const struct timespec *start, const struct timespec *end) {
    const double seconds = (double)(end->tv_sec - start->tv_sec);
    const double nanoseconds = (double)(end->tv_nsec - start->tv_nsec);

    return seconds * 1000.0 + nanoseconds / 1000000.0;
}
