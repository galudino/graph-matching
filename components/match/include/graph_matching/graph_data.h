///
/// \file       graph_data.h
/// \brief      Adjacency-array representation of an undirected weighted graph.
///
/// \details    Adapted from course-provided starter code.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#ifndef GRAPH_MATCHING_GRAPH_DATA_H
#define GRAPH_MATCHING_GRAPH_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Undirected weighted graph stored as adjacency arrays.
///
/// \details    The neighbors of vertex v are index[offset[v]] through
///             index[offset[v] + degree[v] - 1], with the matching edge
///             weights at the same positions in weight[]. Each neighbor list
///             is sorted by descending weight, then ascending vertex ID, so
///             the strongest neighbor comes first.
///
// C header: `using` is not available. NOLINTNEXTLINE(modernize-use-using)
typedef struct graph_data {
    int n_nodes;    ///< Number of vertices.
    int n_edges;    ///< Number of entries in index[] (each undirected edge twice).
    int *offset;    ///< Start of each neighbor list in index[]; n_nodes + 1 entries.
    int *degree;    ///< Number of neighbors of each vertex.
    int *index;     ///< Concatenated neighbor lists, as vertex IDs.
    double *weight; ///< Weight of the edge at the same position in index[].
} graph_data;

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_GRAPH_DATA_H
