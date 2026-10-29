///
/// \file       match.h
/// \brief      Matching status codes and the matching entry points.
///
/// \details    Adapted from match.h in the CS 314 (Rutgers, Fall 2018)
///             Project 3 code package.
///
/// \author     Yanhao Chen (original)
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#ifndef GRAPH_MATCHING_MATCH_H
#define GRAPH_MATCHING_MATCH_H

#include <graph_matching/graph_data.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Status of a vertex that has not been matched yet; every res[] entry starts here.
#define UNMATCHED (-1)

/// Final status of a vertex that ends up without a match.
#define NO_MATCHED_NODES (-2)

/// Marker for a vertex that check_result() has not visited yet.
#define UNVISITED (-3)

///
/// \brief      Run one-way handshaking matching on a thread pool.
///
/// \param      n_threads   Number of worker threads.
/// \param      graph       Graph to match.
/// \param      res         Matching result: res[v] is v's partner, or a
///                         status code. Must hold graph.n_nodes entries,
///                         all initialized to UNMATCHED.
///
void one_way_handshake_wrapper(int n_threads, graph_data graph, int **res);

///
/// \brief      Run N-way handshaking matching on a thread pool.
///
/// \param      n_threads   Number of worker threads.
/// \param      n_ways      Number of hands each vertex extends (at least 2).
/// \param      graph       Graph to match.
/// \param      res         Matching result: res[v] is v's partner, or a
///                         status code. Must hold graph.n_nodes entries,
///                         all initialized to UNMATCHED.
///
void n_ways_handshake_wrapper(int n_threads, int n_ways, graph_data graph, int **res);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_MATCH_H
