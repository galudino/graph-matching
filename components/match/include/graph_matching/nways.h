///
/// \file       nways.h
/// \brief      N-way handshaking matching.
///
/// \details    generate_n_way_graph() and prune_n_way_graph() are
///             implemented by the assignment in nways.c; the thread driver
///             is in nways_wrapper.c. Adapted from course-provided starter
///             code.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#ifndef GRAPH_MATCHING_NWAYS_H
#define GRAPH_MATCHING_NWAYS_H

#include <graph_matching/graph_data.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Arguments for one N-way handshaking worker thread.
///
/// \details    Every thread shares the same arrays; only thread_id differs.
///
typedef struct n_way_thread_data {
    int thread_id;              ///< Index of this thread.
    int thread_num;             ///< Total number of threads.
    pthread_barrier_t *barrier; ///< Barrier that separates the steps of each pass.
    int n_ways;                 ///< Number of hands each vertex extends.
    graph_data graph;           ///< Graph being matched.
    int *res;                   ///< Matching status of every vertex.
    int *strong_neighbor;       ///< Strongest neighbor of every vertex in the N-way graph.
    int *n_way_graph;           ///< N-way graph adjacency lists; n_ways entries per vertex.
    int *n_way_graph_degree;    ///< Degree of every vertex in the N-way graph.
    int *node_to_process;       ///< Vertices processed in the current pass.
    int *node_count;            ///< Unmatched vertices found by each thread.
    int *start_locations;       ///< Exclusive prefix sum of node_count.
    int *new_node_to_process;   ///< Vertices for the next pass.
} n_way_thread_data;

///
/// \brief      Worker thread for N-way handshaking.
///
/// \details    Repeats passes of generate_n_way_graph(), prune_n_way_graph(),
///             check_handshaking(), and the filter until no unmatched vertex
///             remains.
///
/// \param      thread_arg  Points to this thread's n_way_thread_data.
///
/// \return     The number of passes, cast to a pointer.
///
void *n_ways_handshake(void *thread_arg);

///
/// \brief      Build the N-way graph for the vertices in one thread's share.
///
/// \details    The N-way neighbors of vertex v are stored from
///             n_way_graph[v * n_ways] to
///             n_way_graph[v * n_ways + n_way_graph_degree[v] - 1], strongest
///             first.
///
/// \param[in]  thread_id           Index of the calling thread.
/// \param[in]  thread_num          Total number of threads.
/// \param[in]  graph               Graph being matched.
/// \param[in]  n_ways              Number of hands each vertex extends.
/// \param[in]  node_num            Number of vertices in node_to_process.
/// \param[in]  node_to_process     Vertex IDs still being processed; the first
///                                 node_num entries are valid.
/// \param[in]  res                 Matching status of every vertex.
/// \param[out] n_way_graph_degree  Receives each processed vertex's degree in
///                                 the N-way graph; one entry per vertex.
/// \param[out] n_way_graph         Receives each processed vertex's N-way
///                                 neighbors; n_ways entries per vertex.
///
void generate_n_way_graph(int thread_id, int thread_num, graph_data graph, int n_ways, int node_num,
                          const int *node_to_process, const int *res, int *n_way_graph_degree,
                          int *n_way_graph);

///
/// \brief      Prune one-sided edges from the N-way graph and pick each vertex's strongest
/// neighbor.
///
/// \details    Removes edges that do not appear in both endpoints' N-way
///             neighbor lists, then has each vertex in this thread's share
///             extend a hand to its strongest remaining N-way neighbor.
///
/// \param[in]     thread_id           Index of the calling thread.
/// \param[in]     thread_num          Total number of threads.
/// \param[in]     n_ways              Number of hands each vertex extends.
/// \param[in]     node_num            Number of vertices in node_to_process.
/// \param[in]     node_to_process     Vertex IDs still being processed; the
///                                    first node_num entries are valid.
/// \param[in,out] n_way_graph_degree  Degree of every vertex in the N-way graph.
/// \param[in,out] n_way_graph         N-way graph adjacency lists.
/// \param[out]    strong_neighbor     Receives each processed vertex's
///                                    strongest N-way neighbor; one entry per
///                                    vertex.
///
void prune_n_way_graph(int thread_id, int thread_num, int n_ways, int node_num,
                       const int *node_to_process, int *n_way_graph_degree, int *n_way_graph,
                       int *strong_neighbor);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_NWAYS_H
