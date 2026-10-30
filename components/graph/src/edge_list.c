///
/// \file       edge_list.c
/// \brief      Sortable list of weighted edges.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/edge_list.h>

#include <stddef.h>
#include <stdlib.h>

status_code edge_list_init(edge_list *const self, int length) {
    self->edges = NULL;
    self->length = 0;

    if (length < 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    // Allocate at least one edge: malloc(0) may return NULL.
    const size_t capacity = length > 0 ? (size_t)length : 1;
    self->edges = malloc(capacity * sizeof(edge));
    if (self->edges == NULL) {
        return STATUS_OUT_OF_MEMORY;
    }

    self->length = length;
    return STATUS_OK;
}

void edge_list_destroy(edge_list *const self) {
    free(self->edges);
    self->edges = NULL;
    self->length = 0;
}

///
/// \brief      Three-way comparison of two ints, for qsort().
///
/// \param      a           First value.
/// \param      b           Second value.
///
/// \return     Negative, zero, or positive as a is less than, equal to, or
///             greater than b.
///
static int compare_ints(int a, int b) { return (a > b) - (a < b); }

///
/// \brief      Three-way comparison of two weights, heaviest first.
///
/// \param      a           First weight.
/// \param      b           Second weight.
///
/// \return     Negative if a is heavier than b, positive if lighter, zero if equal.
///
static int compare_weights_descending(double a, double b) { return (a < b) - (a > b); }

///
/// \brief      Order edges by source, then target, then heaviest first.
///
/// \param      left        First edge, as qsort() passes it.
/// \param      right       Second edge.
///
/// \return     Negative, zero, or positive as left sorts before, with, or after right.
///
static int compare_source_target_weight(const void *left, const void *right) {
    const edge *a = left;
    const edge *b = right;

    int order = compare_ints(a->source, b->source);
    if (order == 0) {
        order = compare_ints(a->target, b->target);
    }
    if (order == 0) {
        order = compare_weights_descending(a->weight, b->weight);
    }
    return order;
}

///
/// \brief      Order edges by source, then heaviest first, then target.
///
/// \param      left        First edge, as qsort() passes it.
/// \param      right       Second edge.
///
/// \return     Negative, zero, or positive as left sorts before, with, or after right.
///
static int compare_source_weight_target(const void *left, const void *right) {
    const edge *a = left;
    const edge *b = right;

    int order = compare_ints(a->source, b->source);
    if (order == 0) {
        order = compare_weights_descending(a->weight, b->weight);
    }
    if (order == 0) {
        order = compare_ints(a->target, b->target);
    }
    return order;
}

///
/// \brief      Order edges heaviest first, then by source, then target.
///
/// \param      left        First edge, as qsort() passes it.
/// \param      right       Second edge.
///
/// \return     Negative, zero, or positive as left sorts before, with, or after right.
///
static int compare_weight_source_target(const void *left, const void *right) {
    const edge *a = left;
    const edge *b = right;

    int order = compare_weights_descending(a->weight, b->weight);
    if (order == 0) {
        order = compare_ints(a->source, b->source);
    }
    if (order == 0) {
        order = compare_ints(a->target, b->target);
    }
    return order;
}

void edge_list_sort(edge_list *const self, edge_order order) {
    int (*compare)(const void *, const void *);

    switch (order) {
    case EDGE_ORDER_SOURCE_WEIGHT_TARGET:
        compare = compare_source_weight_target;
        break;
    case EDGE_ORDER_WEIGHT_SOURCE_TARGET:
        compare = compare_weight_source_target;
        break;
    case EDGE_ORDER_SOURCE_TARGET_WEIGHT:
    default:
        compare = compare_source_target_weight;
        break;
    }

    qsort(self->edges, (size_t)self->length, sizeof(edge), compare);
}
