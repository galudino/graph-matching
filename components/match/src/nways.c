///
/// \file       nways.c
/// \brief      N-way handshaking matching (assignment, extra credit).
///
/// \details    See spec sections 2.2 (work balancing) and 2.4, in
///             docs/CS314_Fall_2018_Project3.pdf. check_handshaking() from
///             oneway.c is reused for the handshake step.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/nways.h>

// The empty functions below don't write their outputs yet, which clang-tidy
// reports. Delete these NOLINT markers once the functions are implemented.
// NOLINTBEGIN(readability-non-const-parameter)

void generate_n_way_graph(int thread_id, int thread_num, graph_data graph, int n_ways, int node_num,
                          const int *node_to_process, const int *res, int *n_way_graph_degree,
                          int *n_way_graph) {
    // TODO: Implement, then delete these casts.
    (void)thread_id;
    (void)thread_num;
    (void)graph;
    (void)n_ways;
    (void)node_num;
    (void)node_to_process;
    (void)res;
    (void)n_way_graph_degree;
    (void)n_way_graph;
}

void prune_n_way_graph(int thread_id, int thread_num, int n_ways, int node_num,
                       const int *node_to_process, int *n_way_graph_degree, int *n_way_graph,
                       int *strong_neighbor) {
    // TODO: Implement, then delete these casts.
    (void)thread_id;
    (void)thread_num;
    (void)n_ways;
    (void)node_num;
    (void)node_to_process;
    (void)n_way_graph_degree;
    (void)n_way_graph;
    (void)strong_neighbor;
}

// NOLINTEND(readability-non-const-parameter)
