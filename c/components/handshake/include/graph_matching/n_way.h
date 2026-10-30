///
/// \file       n_way.h
/// \brief      The N-way steps that replace one-way's hand extension.
///
/// \details    Assignment (extra credit): implement these functions in
///             n_way.c. See docs/assignment.md, section 5. After them, each
///             pass continues with one_way_check_handshakes().
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_N_WAY_H
#define GRAPH_MATCHING_N_WAY_H

#include <graph_matching/handshake_worker.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Have each vertex in this thread's range extend N hands, to its N strongest available
/// neighbors.
///
/// \details    For each vertex v in worklist_range(), with
///             N = shared->hand_count: writes the first N vertices u in
///             graph_neighbors(v) (strongest first) with
///             partners[u] == VERTEX_UNMATCHED to
///             shared->candidates[v * N] onward, strongest first, and sets
///             shared->candidate_counts[v] to how many were written (0 to N).
///
/// \param      self        Calling thread.
///
void n_way_find_candidates(const handshake_worker *self);

///
/// \brief      Have each vertex in this thread's range extend one hand, to its strongest mutual
/// candidate.
///
/// \details    For each vertex v in worklist_range(), sets
///             shared->strongest_neighbor[v] to:
///             - v's first candidate u (strongest first) that also has v
///               among its own candidates;
///             - VERTEX_UNMATCHED if v has candidates but none of them chose
///               v, so v extends no hand this pass and tries again next pass;
///             - VERTEX_UNMATCHABLE if v has no candidates at all, because
///               every neighbor is already matched or unmatchable.
///
/// \param      self        Calling thread.
///
void n_way_extend_hand(const handshake_worker *self);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_N_WAY_H
