///
/// \file       edge_list.h
/// \brief      Sortable list of weighted edges.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_EDGE_LIST_H
#define GRAPH_MATCHING_EDGE_LIST_H

#include <graph_matching/status_code.h>

#ifdef __cplusplus
extern "C" {
#endif

///
/// \brief      One weighted edge between two vertices.
///
typedef struct edge {
    int source;    ///< First endpoint (0-based vertex ID).
    int target;    ///< Second endpoint (0-based vertex ID).
    double weight; ///< Edge weight.
} edge;

///
/// \brief      Key orders for edge_list_sort().
///
/// \details    Each name lists the keys from most to least significant.
///             Vertex IDs sort ascending; weights sort descending, so the
///             heaviest edge comes first.
///
typedef enum edge_order {
    EDGE_ORDER_SOURCE_TARGET_WEIGHT, ///< By source, then target, then heaviest first.
    EDGE_ORDER_SOURCE_WEIGHT_TARGET, ///< By source, then heaviest first, then target.
    EDGE_ORDER_WEIGHT_SOURCE_TARGET, ///< Heaviest first, then by source, then target.
} edge_order;

///
/// \brief      Fixed-length array of edges.
///
typedef struct edge_list {
    edge *edges; ///< The edges; length entries.
    int length;  ///< Number of edges.
} edge_list;

///
/// \brief      Allocate an edge list.
///
/// \details    The edges' contents are unspecified until written. On
///             failure, self is left empty and safe to pass to edge_list_deinit().
///
/// \param      self        Edge list to initialize.
/// \param      length      Number of edges; at least 0.
///
/// \return     STATUS_OK, STATUS_INVALID_ARGUMENT, or STATUS_OUT_OF_MEMORY.
///
status_code edge_list_init(edge_list *self, int length);

///
/// \brief      Release an edge list's memory.
///
/// \param      self        Edge list to release; left empty.
///
void edge_list_deinit(edge_list *self);

///
/// \brief      Sort the edges in place.
///
/// \param      self        Edge list to sort.
/// \param      order       Key order.
///
void edge_list_sort(edge_list *self, edge_order order);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_EDGE_LIST_H
