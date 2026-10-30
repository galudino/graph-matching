///
/// \file       worklist.h
/// \brief      Splitting the worklist across threads, and removing finished vertices.
///
/// \details    Assignment: implement these functions in worklist.c. See
///             docs/assignment.md, sections 3 and 6.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_WORKLIST_H
#define GRAPH_MATCHING_WORKLIST_H

#include <graph_matching/handshake_worker.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Half-open range of indices, [begin, end).
///
typedef struct index_range {
    int begin; ///< First index in the range.
    int end;   ///< One past the last index; equal to begin if the range is empty.
} index_range;

///
/// \brief      Find the part of the worklist this thread is responsible for.
///
/// \details    Splits indices 0 to active_count - 1 of active_vertices into
///             thread_count contiguous ranges of
///             ceil(active_count / thread_count) indices each; the last
///             ranges may be shorter or empty. Every other assignment
///             function processes exactly the vertices
///             active_vertices[begin] through active_vertices[end - 1].
///
/// \param      self        Calling thread.
///
/// \return     This thread's range of indices into active_vertices.
///
index_range worklist_range(const handshake_worker *self);

///
/// \brief      Count the vertices in this thread's range that are still unmatched.
///
/// \details    Stores the number of vertices v in worklist_range() with
///             partners[v] == VERTEX_UNMATCHED in
///             shared->unmatched_counts[thread_index], which is 0 on entry.
///
/// \param      self        Calling thread.
///
void worklist_count_unmatched(const handshake_worker *self);

///
/// \brief      Copy this thread's still-unmatched vertices into the next worklist.
///
/// \details    shared->write_offsets holds the exclusive prefix sum of
///             unmatched_counts, so this thread's vertices belong in
///             next_active_vertices starting at write_offsets[thread_index].
///             Copies them there in their current order. May use
///             write_offsets[thread_index] as a cursor, but must not change
///             any other entry.
///
/// \param      self        Calling thread.
///
void worklist_compact(const handshake_worker *self);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_WORKLIST_H
