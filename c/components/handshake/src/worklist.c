///
/// \file       worklist.c
/// \brief      Splitting the worklist across threads, and removing finished vertices (assignment).
///
/// \details    See worklist.h for each function's contract, and
///             docs/assignment.md, sections 3 and 6.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/worklist.h>

#include <graph_matching/handshake_worker.h>
#include <graph_matching/matching.h>

index_range worklist_range(const handshake_worker *const self) {
    // TODO: Implement.
    (void)self;
    return (index_range){.begin = 0, .end = 0};
}

void worklist_count_unmatched(const handshake_worker *const self) {
    // TODO: Implement.
    (void)self;
}

void worklist_compact(const handshake_worker *const self) {
    // TODO: Implement.
    (void)self;
}
