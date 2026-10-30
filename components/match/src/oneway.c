///
/// \file       oneway.c
/// \brief      One-way handshaking matching (assignment).
///
/// \details    Each function's inputs and outputs are documented in
///             oneway.h.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/oneway.h>

// The empty functions below don't write their outputs yet, which clang-tidy
// reports. Delete these NOLINT markers once the functions are implemented.
// NOLINTBEGIN(readability-non-const-parameter)

void extend_one_hand(int thread_id, int thread_num, graph_data graph, int node_num,
                     const int *node_to_process, const int *res, int *strong_neighbor) {
    // TODO: Implement, then delete these casts.
    (void)thread_id;
    (void)thread_num;
    (void)graph;
    (void)node_num;
    (void)node_to_process;
    (void)res;
    (void)strong_neighbor;
}

void check_handshaking(int thread_id, int thread_num, int node_num, const int *node_to_process,
                       const int *strong_neighbor, int *res) {
    // TODO: Implement, then delete these casts.
    (void)thread_id;
    (void)thread_num;
    (void)node_num;
    (void)node_to_process;
    (void)strong_neighbor;
    (void)res;
}

// NOLINTEND(readability-non-const-parameter)
