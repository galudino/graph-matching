///
/// \file       stopwatch.h
/// \brief      Elapsed-time measurement with a monotonic clock.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_STOPWATCH_H
#define GRAPH_MATCHING_STOPWATCH_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Measures time elapsed since it was started.
///
typedef struct stopwatch {
    struct timespec start; ///< Monotonic time when stopwatch_start() was called.
} stopwatch;

///
/// \brief      Start, or restart, a stopwatch.
///
/// \param      self        Stopwatch to start.
///
void stopwatch_start(stopwatch *self);

///
/// \brief      Measure the time since stopwatch_start().
///
/// \param      self        Started stopwatch.
///
/// \return     Elapsed time in milliseconds.
///
double stopwatch_elapsed_ms(const stopwatch *self);

///
/// \brief      Compute the time between two monotonic clock readings.
///
/// \param      start       Earlier reading.
/// \param      end         Later reading.
///
/// \return     end - start in milliseconds.
///
double timespec_difference_ms(const struct timespec *start, const struct timespec *end);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_STOPWATCH_H
