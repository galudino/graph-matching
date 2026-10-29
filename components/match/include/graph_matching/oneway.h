///
/// \file       oneway.h
/// \brief      One-way handshaking matching.
///
/// \details    extend_one_hand() and check_handshaking() are implemented by
///             the assignment in oneway.c; the thread driver is in
///             oneway_wrapper.c. Adapted from oneway.h in the CS 314
///             (Rutgers, Fall 2018) Project 3 code package.
///
/// \author     Yanhao Chen (original)
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#ifndef GRAPH_MATCHING_ONEWAY_H
#define GRAPH_MATCHING_ONEWAY_H

#include <graph_matching/graph_data.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Arguments for one one-way handshaking worker thread.
///
/// \details    Every thread shares the same arrays; only thread_id differs.
///
typedef struct one_way_thread_data {
    int thread_id;              ///< Index of this thread.
    int thread_num;             ///< Total number of threads.
    pthread_barrier_t *barrier; ///< Barrier that separates the steps of each pass.
    graph_data graph;           ///< Graph being matched.
    int *res;                   ///< Matching status of every vertex.
    int *strong_neighbor;       ///< Strongest unmatched neighbor of every vertex.
    int *node_to_process;       ///< Vertices processed in the current pass.
    int *node_count;            ///< Unmatched vertices found by each thread.
    int *start_locations;       ///< Exclusive prefix sum of node_count.
    int *new_node_to_process;   ///< Vertices for the next pass.
} one_way_thread_data;

///
/// \brief      Worker thread for one-way handshaking.
///
/// \details    Repeats passes of extend_one_hand(), check_handshaking(), and
///             the filter until no unmatched vertex remains.
///
/// \param      thread_arg  Points to this thread's one_way_thread_data.
///
/// \return     The number of passes, cast to a pointer.
///
void *one_way_handshake(void *thread_arg);

///
/// \brief      Have each vertex in one thread's share pick its strongest unmatched neighbor.
///
/// \param[in]  thread_id       Index of the calling thread.
/// \param[in]  thread_num      Total number of threads.
/// \param[in]  graph           Graph being matched.
/// \param[in]  node_num        Number of vertices in node_to_process.
/// \param[in]  node_to_process Vertex IDs still being processed; the first
///                             node_num entries are valid.
/// \param[in]  res             Matching status of every vertex.
/// \param[out] strong_neighbor Receives each processed vertex's strongest
///                             unmatched neighbor; one entry per vertex.
///
void extend_one_hand(int thread_id, int thread_num, graph_data graph, int node_num,
                     const int *node_to_process, const int *res, int *strong_neighbor);

///
/// \brief      Match the vertices in one thread's share whose hands meet.
///
/// \param[in]  thread_id       Index of the calling thread.
/// \param[in]  thread_num      Total number of threads.
/// \param[in]  node_num        Number of vertices in node_to_process.
/// \param[in]  node_to_process Vertex IDs still being processed; the first
///                             node_num entries are valid.
/// \param[in]  strong_neighbor Strongest unmatched neighbor of every vertex,
///                             from extend_one_hand().
/// \param[out] res             Receives the matching status of each
///                             processed vertex.
///
void check_handshaking(int thread_id, int thread_num, int node_num, const int *node_to_process,
                       const int *strong_neighbor, int *res);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_ONEWAY_H
