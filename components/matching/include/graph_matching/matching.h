///
/// \file       matching.h
/// \brief      A matching of a graph's vertices, with I/O and validation.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_MATCHING_H
#define GRAPH_MATCHING_MATCHING_H

#include <graph_matching/graph.h>
#include <graph_matching/status_code.h>

#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      Partner values that mean a vertex has no partner.
///
typedef enum vertex_status {
    VERTEX_UNMATCHED = -1,   ///< Not matched yet; every vertex starts here.
    VERTEX_UNMATCHABLE = -2, ///< Finished without a partner: no neighbor was available.
} vertex_status;

///
/// \brief      Partner of every vertex in a graph.
///
/// \details    partners[v] is the vertex matched with v, or a vertex_status
///             value. In a valid matching, partners[partners[v]] == v for
///             every matched vertex v.
///
typedef struct matching {
    int vertex_count; ///< Number of vertices.
    int *partners;    ///< Partner or vertex_status of each vertex; vertex_count entries.
} matching;

///
/// \brief      Allocate a matching with every vertex VERTEX_UNMATCHED.
///
/// \details    On failure, self is left empty and safe to destroy.
///
/// \param      self            Matching to initialize.
/// \param      vertex_count    Number of vertices; at least 0.
///
/// \return     STATUS_OK, STATUS_INVALID_ARGUMENT, or STATUS_OUT_OF_MEMORY.
///
status_code matching_init(matching *self, int vertex_count);

///
/// \brief      Release a matching's memory.
///
/// \param      self        Matching to release; left empty.
///
void matching_destroy(matching *self);

///
/// \brief      Match greedily: take edges heaviest first while both endpoints are free.
///
/// \details    Considers each edge once, by descending weight, then
///             ascending smaller endpoint, then ascending larger endpoint.
///             Vertices left without a partner become VERTEX_UNMATCHABLE.
///             Runs on one thread; useful as a baseline for the parallel
///             algorithms.
///
/// \param      self            Matching with every vertex VERTEX_UNMATCHED.
/// \param      matched_graph   Graph to match; same vertex_count as self.
///
/// \return     STATUS_OK, STATUS_INVALID_ARGUMENT, or STATUS_OUT_OF_MEMORY.
///
status_code matching_run_greedy(matching *self, const graph *matched_graph);

///
/// \brief      Check that a finished matching is valid.
///
/// \details    Checks that every vertex has a partner or is
///             VERTEX_UNMATCHABLE (none is still VERTEX_UNMATCHED), and that
///             every match is mutual and is an edge of the graph. Logs the
///             first problem found.
///
/// \param      self            Matching to check.
/// \param      matched_graph   Graph it is a matching of.
///
/// \return     STATUS_OK, or STATUS_INVALID_MATCHING.
///
status_code matching_validate(const matching *self, const graph *matched_graph);

///
/// \brief      Check whether no edge could be added to a finished matching.
///
/// \details    A matching is maximal when no edge joins two vertices that
///             are both without a partner. The greedy baseline and both
///             handshaking algorithms always produce maximal matchings.
///
/// \param      self            Valid matching.
/// \param      matched_graph   Graph it is a matching of.
///
/// \return     True if the matching is maximal.
///
bool matching_is_maximal(const matching *self, const graph *matched_graph);

///
/// \brief      Count the matched pairs.
///
/// \param      self        Matching.
///
/// \return     The number of vertex pairs matched with each other.
///
int matching_pair_count(const matching *self);

///
/// \brief      Add up the weights of the matched edges.
///
/// \param      self            Valid matching.
/// \param      matched_graph   Graph it is a matching of.
///
/// \return     The total weight of the matched edges.
///
double matching_weight(const matching *self, const graph *matched_graph);

///
/// \brief      Write a matching, one partners[] value per line.
///
/// \param      self        Matching to write.
/// \param      path        Path of the output file.
///
/// \return     STATUS_OK, or STATUS_FILE_ERROR.
///
status_code matching_write(const matching *self, const char *path);

///
/// \brief      Read a matching written by matching_write().
///
/// \details    On failure, self is left empty and safe to destroy.
///
/// \param      self            Matching to initialize.
/// \param      path            Path of the file to read.
/// \param      vertex_count    Number of vertices (lines) to read.
///
/// \return     STATUS_OK, STATUS_INVALID_ARGUMENT, STATUS_FILE_ERROR,
///             STATUS_PARSE_ERROR, or STATUS_OUT_OF_MEMORY.
///
status_code matching_read(matching *self, const char *path, int vertex_count);

///
/// \brief      Write each vertex's partner or status in readable form.
///
/// \param      self        Matching to print.
/// \param      stream      Stream to write to, such as stderr.
///
void matching_print(const matching *self, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_MATCHING_H
