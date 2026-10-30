///
/// \file       handshake_worker.h
/// \brief      State shared by the handshaking threads, and each thread's view of it.
///
/// \details    The assignment functions in one_way.h, n_way.h, and
///             worklist.h each take the calling thread's handshake_worker.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_HANDSHAKE_WORKER_H
#define GRAPH_MATCHING_HANDSHAKE_WORKER_H

#include <graph_matching/graph.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Arrays and settings shared by every handshaking thread.
///
/// \details    Arrays indexed by vertex have input_graph->vertex_count
///             entries. Each thread writes only the entries of the vertices
///             (or the per-thread slot) it is responsible for.
///
typedef struct handshake_shared {
    const graph *input_graph;  ///< Graph being matched.
    int *partners;             ///< Partner or vertex_status of each vertex (the result).
    int thread_count;          ///< Number of threads.
    int hand_count;            ///< Hands per vertex: 1 for one-way, N for N-way.
    pthread_barrier_t barrier; ///< Separates the steps of each pass.
    int *strongest_neighbor;   ///< Each vertex's hand this pass: a vertex or a vertex_status.
    int *candidates;           ///< N-way: hand_count candidate slots per vertex.
    int *candidate_counts;     ///< N-way: number of filled candidate slots per vertex.
    int *unmatched_counts;     ///< Unmatched vertices in each thread's range; thread_count entries.
    int *write_offsets; ///< Exclusive prefix sum of unmatched_counts; thread_count + 1 entries.
} handshake_shared;

///
/// \brief      One thread's view of the matching: its index and the current worklist.
///
/// \details    Every thread sees the same active_vertices and
///             active_count; the driver swaps active_vertices and
///             next_active_vertices between passes.
///
typedef struct handshake_worker {
    handshake_shared *shared;  ///< State shared by all threads.
    int thread_index;          ///< This thread's index, from 0 to thread_count - 1.
    int active_count;          ///< Number of vertices in active_vertices.
    int *active_vertices;      ///< Vertices still VERTEX_UNMATCHED, processed this pass.
    int *next_active_vertices; ///< Receives the vertices still unmatched after this pass.
    long pass_index;           ///< Index of the current pass, from 0.
} handshake_worker;

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_HANDSHAKE_WORKER_H
