///
/// \file       filter.h
/// \brief      Removes matched vertices from the work list between passes.
///
/// \details    Implemented by the assignment in filter.c. Adapted from
///             filter.h in the CS 314 (Rutgers, Fall 2018) Project 3 code
///             package.
///
/// \author     Yanhao Chen (original)
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#ifndef GRAPH_MATCHING_FILTER_H
#define GRAPH_MATCHING_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Count the unmatched vertices in one thread's share of the work list.
///
/// \param[in]  thread_id       Index of the calling thread.
/// \param[in]  thread_num      Total number of threads.
/// \param[in]  node_num        Number of vertices in node_to_process.
/// \param[in]  node_to_process Vertex IDs still being processed; the first
///                             node_num entries are valid.
/// \param[in]  res             Matching status of every vertex.
/// \param[out] node_count      node_count[thread_id] receives the count;
///                             thread_num entries.
///
void count_unmatched_vertices(int thread_id, int thread_num, int node_num,
                              const int *node_to_process, const int *res, int *node_count);

///
/// \brief      Copy one thread's unmatched vertices into the next pass's work list.
///
/// \param[in]     thread_id           Index of the calling thread.
/// \param[in]     thread_num          Total number of threads.
/// \param[in]     node_to_process     Vertex IDs processed in this pass; the
///                                    first node_num entries are valid.
/// \param[in,out] start_locations     Where each thread starts writing in
///                                    new_node_to_process: the exclusive
///                                    prefix sum of node_count; thread_num + 1
///                                    entries.
/// \param[in]     res                 Matching status of every vertex.
/// \param[in]     node_num            Number of vertices in node_to_process.
/// \param[out]    new_node_to_process Unmatched vertex IDs for the next pass.
///
void update_remain_nodes_index(int thread_id, int thread_num, const int *node_to_process,
                               int *start_locations, const int *res, int node_num,
                               int *new_node_to_process);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_FILTER_H
