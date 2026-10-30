///
/// \file       prefix_sum.h
/// \brief      Running totals of integer arrays.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_PREFIX_SUM_H
#define GRAPH_MATCHING_PREFIX_SUM_H

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Compute the exclusive prefix sum of an array.
///
/// \details    sums[0] is 0 and sums[i] is values[0] + ... + values[i - 1],
///             so sums[count] holds the total. For example, values {3, 0, 2}
///             give sums {0, 3, 3, 5}.
///
/// \param      values      Input values; count entries.
/// \param      sums        Output; count + 1 entries.
/// \param      count       Number of input values.
///
void prefix_sum_exclusive(const int *values, int *sums, int count);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_PREFIX_SUM_H
