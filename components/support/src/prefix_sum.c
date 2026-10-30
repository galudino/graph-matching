///
/// \file       prefix_sum.c
/// \brief      Running totals of integer arrays.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/prefix_sum.h>

void prefix_sum_exclusive(const int *values, int *sums, int count) {
    sums[0] = 0;
    for (int i = 0; i < count; ++i) {
        sums[i + 1] = sums[i] + values[i];
    }
}
