///
/// \file       handshake.h
/// \brief      Parallel graph matching by one-way and N-way handshaking.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_HANDSHAKE_H
#define GRAPH_MATCHING_HANDSHAKE_H

#include <graph_matching/graph.h>
#include <graph_matching/matching.h>
#include <graph_matching/status_code.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Settings for handshake_match().
///
typedef struct handshake_options {
    int thread_count; ///< Number of threads; at least 1.
    int hand_count;   ///< Hands per vertex: 1 for one-way, 2 or more for N-way.
} handshake_options;

///
/// \brief      Measurements from a handshake_match() run.
///
typedef struct handshake_statistics {
    long pass_count;   ///< Number of passes run, including the last one, which left no vertex
                       ///< unmatched.
    double elapsed_ms; ///< Wall-clock time of the matching, in milliseconds.
} handshake_statistics;

///
/// \brief      Match a graph by one-way or N-way handshaking.
///
/// \details    Runs passes until every vertex has a partner or is
///             VERTEX_UNMATCHABLE. Each pass, every remaining vertex extends
///             a hand (one-way) or N hands (N-way), vertices whose hands meet
///             are matched, and the rest move on to the next pass. Aborts
///             the process if a thread cannot be created, since the other
///             threads would wait for it forever.
///
/// \param      self            Result; vertex_count must match the graph's,
///                             with every vertex VERTEX_UNMATCHED.
/// \param      input_graph     Graph to match.
/// \param      options         Thread and hand counts.
/// \param      statistics      Receives the pass count and time; may be NULL.
///
/// \return     STATUS_OK, STATUS_INVALID_ARGUMENT, or STATUS_OUT_OF_MEMORY.
///
status_code handshake_match(matching *self, const graph *input_graph,
                            const handshake_options *options, handshake_statistics *statistics);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_HANDSHAKE_H
