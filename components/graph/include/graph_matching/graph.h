///
/// \file       graph.h
/// \brief      Undirected weighted graph stored as adjacency arrays.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_GRAPH_H
#define GRAPH_MATCHING_GRAPH_H

#include <graph_matching/edge_list.h>
#include <graph_matching/status_code.h>

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Undirected weighted graph stored as adjacency arrays.
///
/// \details    The neighbors of vertex v are
///             neighbors[neighbor_offsets[v]] through
///             neighbors[neighbor_offsets[v + 1] - 1], with each edge's weight
///             at the same position in weights[]. Each neighbor list is
///             sorted strongest first: by descending weight, then ascending
///             vertex ID. Every edge appears in both of its endpoints' lists.
///
///             Use graph_degree(), graph_neighbors(), and
///             graph_neighbor_weights() rather than indexing the arrays.
///
typedef struct graph {
    int vertex_count;      ///< Number of vertices, numbered 0 to vertex_count - 1.
    int edge_count;        ///< Number of undirected edges.
    int *neighbor_offsets; ///< Start of each neighbor list; vertex_count + 1 entries.
    int *neighbors;        ///< All neighbor lists, concatenated; 2 * edge_count entries.
    double *weights;       ///< Weight of each entry in neighbors; 2 * edge_count entries.
} graph;

///
/// \brief      Build a graph from a list of edges.
///
/// \details    Each edge connects its source and target in both directions.
///             Self-loops are dropped, and when the same pair appears more
///             than once, the heaviest copy is kept. On failure, self is
///             left empty and safe to destroy.
///
/// \param      self            Graph to initialize.
/// \param      vertex_count    Number of vertices; at least 0.
/// \param      edges           Edges, with endpoints from 0 to vertex_count - 1.
///
/// \return     STATUS_OK, STATUS_INVALID_ARGUMENT if an endpoint is out of
///             range, or STATUS_OUT_OF_MEMORY.
///
status_code graph_init_from_edges(graph *self, int vertex_count, const edge_list *edges);

///
/// \brief      Read a graph from a Matrix Market coordinate file.
///
/// \details    Entry (i, j) becomes an edge between vertices i - 1 and j - 1,
///             with the entry's value as its weight, or 1 for a pattern
///             matrix. See graph_init_from_edges() for how self-loops and
///             duplicates are handled. On failure, self is left empty and
///             safe to destroy.
///
/// \param      self        Graph to initialize.
/// \param      path        Path to the .mtx file.
///
/// \return     STATUS_OK, STATUS_FILE_ERROR, STATUS_PARSE_ERROR, or
///             STATUS_OUT_OF_MEMORY.
///
status_code graph_read_matrix_market(graph *self, const char *path);

///
/// \brief      Release a graph's memory.
///
/// \param      self        Graph to release; left empty.
///
void graph_destroy(graph *self);

///
/// \brief      Write every vertex's weighted neighbor list.
///
/// \param      self        Graph to print.
/// \param      stream      Stream to write to, such as stderr.
///
void graph_print(const graph *self, FILE *stream);

///
/// \brief      Number of neighbors of a vertex.
///
/// \param      self        Graph.
/// \param      vertex      Vertex ID, from 0 to vertex_count - 1.
///
/// \return     The vertex's degree.
///
static inline int graph_degree(const graph *const self, int vertex) {
    return self->neighbor_offsets[vertex + 1] - self->neighbor_offsets[vertex];
}

///
/// \brief      Neighbors of a vertex, strongest first.
///
/// \param      self        Graph.
/// \param      vertex      Vertex ID, from 0 to vertex_count - 1.
///
/// \return     graph_degree(self, vertex) neighbor IDs.
///
static inline const int *graph_neighbors(const graph *const self, int vertex) {
    return self->neighbors + self->neighbor_offsets[vertex];
}

///
/// \brief      Weights of the edges to a vertex's neighbors.
///
/// \param      self        Graph.
/// \param      vertex      Vertex ID, from 0 to vertex_count - 1.
///
/// \return     graph_degree(self, vertex) weights, in the same order as
///             graph_neighbors().
///
static inline const double *graph_neighbor_weights(const graph *const self, int vertex) {
    return self->weights + self->neighbor_offsets[vertex];
}

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_GRAPH_H
