///
/// \file       one_way.h
/// \brief      The two steps of a one-way handshaking pass.
///
/// \details    Assignment: implement these functions in one_way.c. See
///             docs/assignment.md, section 4. N-way handshaking reuses
///             one_way_check_handshakes().
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_ONE_WAY_H
#define GRAPH_MATCHING_ONE_WAY_H

#include <graph_matching/handshake_worker.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Have each vertex in this thread's range extend a hand to its strongest available
/// neighbor.
///
/// \details    For each vertex v in worklist_range(), sets
///             shared->strongest_neighbor[v] to the first vertex u in
///             graph_neighbors(v) (strongest first) with
///             partners[u] == VERTEX_UNMATCHED, or to VERTEX_UNMATCHABLE if
///             there is none.
///
/// \param      self        Calling thread.
///
void one_way_extend_hand(const handshake_worker *self);

///
/// \brief      Match the vertices in this thread's range whose hands were shaken.
///
/// \details    For each vertex v in worklist_range(), with
///             s = shared->strongest_neighbor[v]:
///             - if s is VERTEX_UNMATCHABLE, sets partners[v] to
///               VERTEX_UNMATCHABLE;
///             - if s is a vertex and strongest_neighbor[s] == v, sets
///               partners[v] to s;
///             - otherwise (s is a vertex that chose someone else, or N-way's
///               VERTEX_UNMATCHED for "no hand this pass"), leaves
///               partners[v] VERTEX_UNMATCHED for the next pass.
///
/// \param      self        Calling thread.
///
void one_way_check_handshakes(const handshake_worker *self);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_ONE_WAY_H
