///
/// \file       edge_list.c
/// \brief      Sortable list of weighted edges.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/edge_list.h>

#include <stdbool.h>
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

void edge_list_deinit(edge_list *const self) {
    free(self->edges);
    self->edges = NULL;
    self->length = 0;
}

///
/// \brief      Check whether one edge sorts before another.
///
/// \details    Vertex IDs compare ascending and weights descending, in the
///             key order given. Inlined into the sort's inner loops, where it
///             runs hundreds of millions of times on large graphs; calling a
///             comparator through qsort() made reading graphs about twice as
///             slow.
///
/// \param      a           First edge.
/// \param      b           Second edge.
/// \param      order       Key order.
///
/// \return     True if a sorts strictly before b.
///
static inline bool edge_before(const edge *a, const edge *b, edge_order order) {
    switch (order) {
    case EDGE_ORDER_SOURCE_WEIGHT_TARGET:
        if (a->source != b->source) {
            return a->source < b->source;
        }
        if (a->weight != b->weight) {
            return a->weight > b->weight;
        }
        return a->target < b->target;

    case EDGE_ORDER_WEIGHT_SOURCE_TARGET:
        if (a->weight != b->weight) {
            return a->weight > b->weight;
        }
        if (a->source != b->source) {
            return a->source < b->source;
        }
        return a->target < b->target;

    case EDGE_ORDER_SOURCE_TARGET_WEIGHT:
    default:
        if (a->source != b->source) {
            return a->source < b->source;
        }
        if (a->target != b->target) {
            return a->target < b->target;
        }
        return a->weight > b->weight;
    }
}

///
/// \brief      Quicksort edges[left] through edges[right] in place.
///
/// \details    Partitions around the middle edge, then recurses into the
///             smaller part and loops on the larger, so the stack depth stays
///             logarithmic.
///
/// \param      edges       Edges to sort.
/// \param      left        First index of the range.
/// \param      right       Last index of the range.
/// \param      order       Key order.
///
static void edge_quicksort(edge *edges, int left, int right, edge_order order) {
    while (left < right) {
        const edge pivot = edges[left + (right - left) / 2];
        int i = left;
        int j = right;

        while (i <= j) {
            while (edge_before(&edges[i], &pivot, order)) {
                ++i;
            }
            while (edge_before(&pivot, &edges[j], order)) {
                --j;
            }
            if (i <= j) {
                const edge swapped = edges[i];
                edges[i] = edges[j];
                edges[j] = swapped;
                ++i;
                --j;
            }
        }

        if (j - left < right - i) {
            edge_quicksort(edges, left, j, order);
            left = i;
        } else {
            edge_quicksort(edges, i, right, order);
            right = j;
        }
    }
}

void edge_list_sort(edge_list *const self, edge_order order) {
    edge_quicksort(self->edges, 0, self->length - 1, order);
}
