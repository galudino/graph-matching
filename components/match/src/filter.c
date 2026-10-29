///
/// \file       filter.c
/// \brief      Filtering matched vertices between passes (assignment).
///
/// \details    See spec sections 2.2 (work balancing) and 2.5, in
///             docs/CS314_Fall_2018_Project3.pdf.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/filter.h>

// The empty functions below don't write their outputs yet, which clang-tidy
// reports. Delete these NOLINT markers once the functions are implemented.
// NOLINTBEGIN(readability-non-const-parameter)

void count_unmatched_vertices(int thread_id, int thread_num, int node_num,
                              const int *node_to_process, const int *res, int *node_count) {
    // TODO: Implement, then delete these casts.
    (void)thread_id;
    (void)thread_num;
    (void)node_num;
    (void)node_to_process;
    (void)res;
    (void)node_count;
}

void update_remain_nodes_index(int thread_id, int thread_num, const int *node_to_process,
                               int *start_locations, const int *res, int node_num,
                               int *new_node_to_process) {
    // TODO: Implement, then delete these casts.
    (void)thread_id;
    (void)thread_num;
    (void)node_to_process;
    (void)start_locations;
    (void)res;
    (void)node_num;
    (void)new_node_to_process;
}

// NOLINTEND(readability-non-const-parameter)
