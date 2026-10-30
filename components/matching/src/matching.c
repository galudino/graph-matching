///
/// \file       matching.c
/// \brief      A matching of a graph's vertices, with I/O and validation.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/matching.h>

#include <graph_matching/edge_list.h>
#include <graph_matching/graph.h>
#include <graph_matching/log.h>
#include <graph_matching/status_code.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

status_code matching_init(matching *const self, int vertex_count) {
    self->vertex_count = 0;
    self->partners = NULL;

    if (vertex_count < 0) {
        return STATUS_INVALID_ARGUMENT;
    }

    // Allocate at least one entry: malloc(0) may return NULL.
    const size_t capacity = vertex_count > 0 ? (size_t)vertex_count : 1;
    self->partners = malloc(capacity * sizeof(int));
    if (self->partners == NULL) {
        return STATUS_OUT_OF_MEMORY;
    }

    self->vertex_count = vertex_count;
    for (int v = 0; v < vertex_count; ++v) {
        self->partners[v] = VERTEX_UNMATCHED;
    }

    return STATUS_OK;
}

void matching_destroy(matching *const self) {
    free(self->partners);
    self->partners = NULL;
    self->vertex_count = 0;
}

status_code matching_run_greedy(matching *const self, const graph *matched_graph) {
    if (self->vertex_count != matched_graph->vertex_count) {
        return STATUS_INVALID_ARGUMENT;
    }

    // List each edge once, with the smaller endpoint as its source.
    edge_list edges;
    const status_code status = edge_list_init(&edges, matched_graph->edge_count);
    if (status != STATUS_OK) {
        return status;
    }

    int length = 0;
    for (int u = 0; u < matched_graph->vertex_count; ++u) {
        const int degree = graph_degree(matched_graph, u);
        const int *neighbors = graph_neighbors(matched_graph, u);
        const double *weights = graph_neighbor_weights(matched_graph, u);

        for (int i = 0; i < degree; ++i) {
            if (u < neighbors[i]) {
                edges.edges[length++] =
                    (edge){.source = u, .target = neighbors[i], .weight = weights[i]};
            }
        }
    }

    edge_list_sort(&edges, EDGE_ORDER_WEIGHT_SOURCE_TARGET);

    for (int i = 0; i < edges.length; ++i) {
        const edge e = edges.edges[i];
        if (self->partners[e.source] == VERTEX_UNMATCHED &&
            self->partners[e.target] == VERTEX_UNMATCHED) {
            self->partners[e.source] = e.target;
            self->partners[e.target] = e.source;
        }
    }

    for (int v = 0; v < self->vertex_count; ++v) {
        if (self->partners[v] == VERTEX_UNMATCHED) {
            self->partners[v] = VERTEX_UNMATCHABLE;
        }
    }

    edge_list_destroy(&edges);
    return STATUS_OK;
}

///
/// \brief      Check whether two vertices are joined by an edge.
///
/// \param      matched_graph   Graph to search.
/// \param      u               First vertex.
/// \param      v               Second vertex.
///
/// \return     True if v is one of u's neighbors.
///
static bool has_edge(const graph *matched_graph, int u, int v) {
    const int degree = graph_degree(matched_graph, u);
    const int *neighbors = graph_neighbors(matched_graph, u);

    for (int i = 0; i < degree; ++i) {
        if (neighbors[i] == v) {
            return true;
        }
    }
    return false;
}

status_code matching_validate(const matching *const self, const graph *matched_graph) {
    if (self->vertex_count != matched_graph->vertex_count) {
        loge("The matching has %d vertices, but the graph has %d.\n", self->vertex_count,
             matched_graph->vertex_count);
        return STATUS_INVALID_MATCHING;
    }

    for (int v = 0; v < self->vertex_count; ++v) {
        const int partner = self->partners[v];

        if (partner == VERTEX_UNMATCHED) {
            loge("Vertex %d is still VERTEX_UNMATCHED.\n", v);
            return STATUS_INVALID_MATCHING;
        }

        if (partner == VERTEX_UNMATCHABLE) {
            continue;
        }

        if (partner < 0 || partner >= self->vertex_count || partner == v) {
            loge("Vertex %d has invalid partner %d.\n", v, partner);
            return STATUS_INVALID_MATCHING;
        }

        if (self->partners[partner] != v) {
            loge("Vertex %d is matched with %d, but %d is matched with %d.\n", v, partner, partner,
                 self->partners[partner]);
            return STATUS_INVALID_MATCHING;
        }

        if (v < partner && !has_edge(matched_graph, v, partner)) {
            loge("Vertices %d and %d are matched but share no edge.\n", v, partner);
            return STATUS_INVALID_MATCHING;
        }
    }

    return STATUS_OK;
}

bool matching_is_maximal(const matching *const self, const graph *matched_graph) {
    for (int v = 0; v < self->vertex_count; ++v) {
        if (self->partners[v] >= 0) {
            continue;
        }

        const int degree = graph_degree(matched_graph, v);
        const int *neighbors = graph_neighbors(matched_graph, v);
        for (int i = 0; i < degree; ++i) {
            if (self->partners[neighbors[i]] < 0) {
                return false;
            }
        }
    }

    return true;
}

int matching_pair_count(const matching *const self) {
    int matched_vertices = 0;
    for (int v = 0; v < self->vertex_count; ++v) {
        if (self->partners[v] >= 0) {
            ++matched_vertices;
        }
    }
    return matched_vertices / 2;
}

double matching_weight(const matching *const self, const graph *matched_graph) {
    double total = 0;

    for (int v = 0; v < self->vertex_count; ++v) {
        const int partner = self->partners[v];
        if (partner <= v) {
            // Count each pair once, from its smaller vertex.
            continue;
        }

        const int degree = graph_degree(matched_graph, v);
        const int *neighbors = graph_neighbors(matched_graph, v);
        const double *weights = graph_neighbor_weights(matched_graph, v);
        for (int i = 0; i < degree; ++i) {
            if (neighbors[i] == partner) {
                total += weights[i];
                break;
            }
        }
    }

    return total;
}

status_code matching_write(const matching *const self, const char *path) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        return STATUS_FILE_ERROR;
    }

    for (int v = 0; v < self->vertex_count; ++v) {
        fprintf(file, "%d\n", self->partners[v]);
    }

    return fclose(file) == 0 ? STATUS_OK : STATUS_FILE_ERROR;
}

status_code matching_read(matching *const self, const char *path, int vertex_count) {
    status_code status = matching_init(self, vertex_count);
    if (status != STATUS_OK) {
        return status;
    }

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        matching_destroy(self);
        return STATUS_FILE_ERROR;
    }

    for (int v = 0; v < vertex_count; ++v) {
        if (fscanf(file, "%d", &self->partners[v]) != 1) {
            status = STATUS_PARSE_ERROR;
            break;
        }
    }

    fclose(file);

    if (status != STATUS_OK) {
        matching_destroy(self);
    }
    return status;
}

void matching_print(const matching *const self, FILE *stream) {
    for (int v = 0; v < self->vertex_count; ++v) {
        const int partner = self->partners[v];

        if (partner == VERTEX_UNMATCHED) {
            fprintf(stream, "%d -> unmatched\n", v);
        } else if (partner == VERTEX_UNMATCHABLE) {
            fprintf(stream, "%d -> unmatchable\n", v);
        } else {
            fprintf(stream, "%d -> %d\n", v, partner);
        }
    }
}
