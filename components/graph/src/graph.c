///
/// \file       graph.c
/// \brief      Undirected weighted graph stored as adjacency arrays.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/graph.h>

#include <graph_matching/edge_list.h>
#include <graph_matching/prefix_sum.h>
#include <graph_matching/status_code.h>

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// mmio.h uses FILE without including <stdio.h> itself.
#include <mmio.h>

///
/// \brief      Set a graph to the empty state, without freeing anything.
///
/// \param      self        Graph to reset.
///
static void graph_reset(graph *const self) {
    self->vertex_count = 0;
    self->edge_count = 0;
    self->neighbor_offsets = NULL;
    self->neighbors = NULL;
    self->weights = NULL;
}

///
/// \brief      Copy edges with the smaller endpoint as the source, dropping self-loops.
///
/// \param      edges           Edges to copy.
/// \param      vertex_count    Number of vertices, for range checks.
/// \param      result          Receives the copy.
///
/// \return     STATUS_OK, STATUS_INVALID_ARGUMENT, or STATUS_OUT_OF_MEMORY.
///
static status_code normalized_copy(const edge_list *edges, int vertex_count, edge_list *result) {
    status_code status = edge_list_init(result, edges->length);
    if (status != STATUS_OK) {
        return status;
    }

    int length = 0;
    for (int i = 0; i < edges->length; ++i) {
        const edge e = edges->edges[i];

        if (e.source < 0 || e.source >= vertex_count || e.target < 0 || e.target >= vertex_count) {
            edge_list_deinit(result);
            return STATUS_INVALID_ARGUMENT;
        }

        if (e.source == e.target) {
            continue;
        }

        const int smaller = e.source < e.target ? e.source : e.target;
        const int larger = e.source < e.target ? e.target : e.source;
        result->edges[length++] = (edge){.source = smaller, .target = larger, .weight = e.weight};
    }

    result->length = length;
    return STATUS_OK;
}

///
/// \brief      Build a graph from normalized edges, taking ownership of them.
///
/// \details    Every edge must have source < target, both in range. The
///             edges are sorted in place and released, whether or not the
///             build succeeds.
///
/// \param      self                Graph to initialize; already reset.
/// \param      vertex_count        Number of vertices.
/// \param      undirected_edges    Normalized edges; released on return.
///
/// \return     STATUS_OK, STATUS_INVALID_ARGUMENT, or STATUS_OUT_OF_MEMORY.
///
static status_code graph_build(graph *const self, int vertex_count, edge_list *undirected_edges) {
    edge_list undirected = *undirected_edges;
    *undirected_edges = (edge_list){.edges = NULL, .length = 0};

    if (undirected.length > INT_MAX / 2) {
        edge_list_deinit(&undirected);
        return STATUS_INVALID_ARGUMENT;
    }

    // Sorting puts copies of the same pair together, heaviest first.
    edge_list_sort(&undirected, EDGE_ORDER_SOURCE_TARGET_WEIGHT);

    // Store each distinct edge in both directions.
    edge_list directed;
    const status_code status = edge_list_init(&directed, undirected.length * 2);
    if (status != STATUS_OK) {
        edge_list_deinit(&undirected);
        return status;
    }

    int edge_count = 0;
    for (int i = 0; i < undirected.length; ++i) {
        const edge e = undirected.edges[i];
        if (i > 0 && e.source == undirected.edges[i - 1].source &&
            e.target == undirected.edges[i - 1].target) {
            continue;
        }

        const size_t slot = 2 * (size_t)edge_count;
        directed.edges[slot] = e;
        directed.edges[slot + 1] =
            (edge){.source = e.target, .target = e.source, .weight = e.weight};
        ++edge_count;
    }
    directed.length = 2 * edge_count;
    edge_list_deinit(&undirected);

    // Group by vertex, strongest neighbor first.
    edge_list_sort(&directed, EDGE_ORDER_SOURCE_WEIGHT_TARGET);

    const size_t entries = directed.length > 0 ? (size_t)directed.length : 1;
    int *degrees = calloc((size_t)vertex_count + 1, sizeof(int));
    self->neighbor_offsets = malloc(((size_t)vertex_count + 1) * sizeof(int));
    self->neighbors = malloc(entries * sizeof(int));
    self->weights = malloc(entries * sizeof(double));

    if (degrees == NULL || self->neighbor_offsets == NULL || self->neighbors == NULL ||
        self->weights == NULL) {
        free(degrees);
        edge_list_deinit(&directed);
        graph_deinit(self);
        return STATUS_OUT_OF_MEMORY;
    }

    for (int i = 0; i < directed.length; ++i) {
        degrees[directed.edges[i].source]++;
        self->neighbors[i] = directed.edges[i].target;
        self->weights[i] = directed.edges[i].weight;
    }

    prefix_sum_exclusive(degrees, self->neighbor_offsets, vertex_count);

    self->vertex_count = vertex_count;
    self->edge_count = edge_count;

    free(degrees);
    edge_list_deinit(&directed);
    return STATUS_OK;
}

status_code graph_init_from_edges(graph *const self, int vertex_count, const edge_list *edges) {
    graph_reset(self);

    if (vertex_count < 0 || edges->length > INT_MAX / 2) {
        return STATUS_INVALID_ARGUMENT;
    }

    edge_list undirected;
    const status_code status = normalized_copy(edges, vertex_count, &undirected);
    if (status != STATUS_OK) {
        return status;
    }

    return graph_build(self, vertex_count, &undirected);
}

///
/// \brief      Read one entry's weight, or 1 if the entry has none.
///
/// \details    Call right after reading the entry's row and column.
///
/// \param      file        Matrix Market file being read.
/// \param      weight      Receives the weight.
///
/// \return     STATUS_OK, or STATUS_PARSE_ERROR if a weight is present but
///             cannot be parsed.
///
static status_code read_entry_weight(FILE *file, double *weight) {
    const int next = fgetc(file);

    if (next == '\n' || next == EOF) {
        *weight = 1;
        return STATUS_OK;
    }

    return fscanf(file, "%lg\n", weight) == 1 ? STATUS_OK : STATUS_PARSE_ERROR;
}

///
/// \brief      Read the entries of an open Matrix Market file.
///
/// \details    Stores each entry as an edge with the smaller vertex as its
///             source (0-based), skipping self-loops: the form graph_build()
///             takes, so no copy is needed.
///
/// \param      file            File positioned at the start.
/// \param      vertex_count    Receives the number of vertices.
/// \param      edges           Receives the edges.
///
/// \return     STATUS_OK, STATUS_PARSE_ERROR, or STATUS_OUT_OF_MEMORY.
///
static status_code read_entries(FILE *file, int *vertex_count, edge_list *edges) {
    MM_typecode type;
    if (mm_read_banner(file, &type) != 0 || !mm_is_sparse(type) || mm_is_complex(type)) {
        return STATUS_PARSE_ERROR;
    }

    int rows = 0;
    int columns = 0;
    int entries = 0;
    if (mm_read_mtx_crd_size(file, &rows, &columns, &entries) != 0 || rows < 0 || columns < 0 ||
        entries < 0) {
        return STATUS_PARSE_ERROR;
    }

    *vertex_count = rows >= columns ? rows : columns;

    const status_code status = edge_list_init(edges, entries);
    if (status != STATUS_OK) {
        return status;
    }

    int length = 0;
    for (int i = 0; i < entries; ++i) {
        int row = 0;
        int column = 0;
        double weight = 0;

        if (fscanf(file, "%d %d", &row, &column) != 2 || row < 1 || row > *vertex_count ||
            column < 1 || column > *vertex_count || read_entry_weight(file, &weight) != STATUS_OK) {
            edge_list_deinit(edges);
            return STATUS_PARSE_ERROR;
        }

        if (row == column) {
            continue;
        }

        const int smaller = row < column ? row : column;
        const int larger = row < column ? column : row;
        edges->edges[length++] =
            (edge){.source = smaller - 1, .target = larger - 1, .weight = weight};
    }

    edges->length = length;
    return STATUS_OK;
}

status_code graph_init_from_matrix_market(graph *const self, const char *path) {
    graph_reset(self);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return STATUS_FILE_ERROR;
    }

    int vertex_count = 0;
    edge_list edges;
    status_code status = read_entries(file, &vertex_count, &edges);
    fclose(file);

    if (status != STATUS_OK) {
        return status;
    }

    return graph_build(self, vertex_count, &edges);
}

void graph_deinit(graph *const self) {
    free(self->neighbor_offsets);
    free(self->neighbors);
    free(self->weights);
    graph_reset(self);
}

void graph_print(const graph *const self, FILE *stream) {
    fprintf(stream, "%d vertices, %d edges\n", self->vertex_count, self->edge_count);

    for (int v = 0; v < self->vertex_count; ++v) {
        const int degree = graph_degree(self, v);
        const int *neighbors = graph_neighbors(self, v);
        const double *weights = graph_neighbor_weights(self, v);

        fprintf(stream, "vertex %d, degree %d:", v, degree);
        for (int i = 0; i < degree; ++i) {
            fprintf(stream, " (%d, %.4f)", neighbors[i], weights[i]);
        }
        fprintf(stream, "\n");
    }
}
