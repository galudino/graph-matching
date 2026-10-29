///
/// \file       utils.c
/// \brief      Diagnostics, timing, prefix sums, and graph and result I/O.
///
/// \details    Adapted from course-provided starter code.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/utils.h>

#include <graph_matching/match.h>
#include <graph_matching/sort.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// mmio.h uses FILE without including <stdio.h> itself.
#include <mmio.h>

int err(const char *level, const char *func, int line, const char *fmt, ...) {
    char message[1024];

    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof message, fmt, args);
    va_end(args);

    return fprintf(stderr, "\n[%s:%s:%d] %s", level, func, line, message);
}

void set_time(struct timespec *time) { clock_gettime(CLOCK_MONOTONIC, time); }

double get_time(const struct timespec *start_time, const struct timespec *end_time) {
    const double seconds = (double)(end_time->tv_sec - start_time->tv_sec);
    const double nanoseconds = (double)(end_time->tv_nsec - start_time->tv_nsec);

    return seconds * 1000.0 + nanoseconds / 1000000.0;
}

void print_usage(const char *program) {
    fprintf(stderr,
            "Usage: %s [input-file] [output-file] [method] [# of hands] [# of threads]\n"
            "       e.g. %s input.mtx output oneWay 4\n"
            "            %s input.mtx output nWays 2 4\n"
            "       [input file]: input file (matrix market format)\n"
            "       [output file]: output file\n"
            "       [method]: oneWay: one-way handshaking method\n"
            "                 nWays: n-way handshaking method\n"
            "       [# hands]: only needed when [method] = nWays; at least 2\n"
            "       [# threads]: number of active threads\n",
            program, program, program);
}

void swap_int_array(int **array_a, int **array_b) {
    int *tmp = *array_a;
    *array_a = *array_b;
    *array_b = tmp;
}

void exclusive_prefix_sum(const int *num, int *sum, int size) {
    sum[0] = 0;
    for (int i = 1; i <= size; i++) {
        sum[i] = sum[i - 1] + num[i - 1];
    }
}

void inclusive_prefix_sum(const int *num, int *sum, int size) {
    sum[0] = num[0];
    for (int i = 1; i < size; i++) {
        sum[i] = sum[i - 1] + num[i];
    }
}

void print_graph(graph_data graph) {
    fprintf(stderr, "%d Nodes %d Edges\n", graph.n_nodes, graph.n_edges);

    for (int i = 0; i < graph.n_nodes; i++) {
        fprintf(stderr, "Node %d | Degree: %d | ", i, graph.degree[i]);

        for (int j = graph.offset[i]; j < graph.offset[i + 1]; j++) {
            fprintf(stderr, "(%d, %.4f) ", graph.index[j], graph.weight[j]);
        }

        fprintf(stderr, "\n");
    }
}

void print_result(const int *res, int n_nodes) {
    for (int i = 0; i < n_nodes; i++) {
        if (res[i] == NO_MATCHED_NODES) {
            fprintf(stderr, "%d -> NO MATCHED NODE\n", i);
        } else if (res[i] == UNMATCHED) {
            fprintf(stderr, "%d -> UNMATCHED\n", i);
        } else {
            fprintf(stderr, "%d -> %d\n", i, res[i]);
        }
    }
}

void write_match_result(const char *file_name, const int *res, int n_nodes) {
    FILE *f = fopen(file_name, "w");
    if (f == NULL) {
        loge("Unable to open the output file.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Start writing matching results to '%s'...", file_name);

    for (int i = 0; i < n_nodes; i++) {
        fprintf(f, "%d\n", res[i]);
    }

    fclose(f);

    fprintf(stderr, "*** SUCCESS ***\n");
}

///
/// \brief      Allocate an array, never asking malloc() for zero bytes.
///
/// \details    A matrix whose entries are all self-loops has no edges, and
///             malloc(0) may return NULL. Allocating at least one element
///             keeps every array valid to pass to free().
///
/// \param      count       Number of elements.
/// \param      size        Size of one element.
///
/// \return     The allocated array.
///
static void *allocate_array(size_t count, size_t size) {
    return malloc((count > 0 ? count : 1) * size);
}

///
/// \brief      Read one entry's weight, or default it to 1 for a pattern matrix.
///
/// \details    Call right after reading the entry's row and column. Exits
///             the program if a weight is present but cannot be parsed.
///
/// \param      f           Matrix Market file being read.
/// \param      weight      Receives the weight.
///
static void read_entry_weight(FILE *f, double *weight) {
    const int next = fgetc(f);

    if (next == '\n' || next == EOF) {
        *weight = 1;
    } else if (fscanf(f, "%lg\n", weight) != 1) {
        loge("Unable to read an edge weight from the Matrix Market file.\n");
        exit(EXIT_FAILURE);
    }
}

///
/// \brief      Write a sequential greedy matching to seq.out, then exit.
///
/// \details    Matches edges in order of descending weight whenever both
///             endpoints are free. Used when RUN_SEQUENTIAL is nonzero, to
///             produce a reference result.
///
/// \param      row         Source vertex of each entry in graph->index.
/// \param      graph       Graph whose index[] and weight[] are filled in.
///
static void write_sequential_matching(int *row, graph_data *graph) {
    fprintf(stderr, "\nEnable sequential matching.\n");

    sort_wrc(row, graph->index, graph->weight, graph->n_edges);

    FILE *f = fopen("seq.out", "w");
    if (f == NULL) {
        loge("Unable to open the sequential output file.\n");
        exit(EXIT_FAILURE);
    }

    int *seq_res = malloc((size_t)graph->n_nodes * sizeof(int));
    for (int i = 0; i < graph->n_nodes; i++) {
        seq_res[i] = NO_MATCHED_NODES;
    }

    for (int i = 0; i < graph->n_edges; i++) {
        if (seq_res[row[i]] == NO_MATCHED_NODES && seq_res[graph->index[i]] == NO_MATCHED_NODES) {
            seq_res[row[i]] = graph->index[i];
            seq_res[graph->index[i]] = row[i];
        }
    }

    for (int i = 0; i < graph->n_nodes; i++) {
        fprintf(f, "%d\n", seq_res[i]);
    }

    fclose(f);
    free(seq_res);

    exit(EXIT_SUCCESS);
}

void read_mm(const char *file_name, graph_data *graph) {
    fprintf(stderr, "Start reading matrix '%s'...", file_name);

    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        loge("Unable to open the input Matrix Market file.\n");
        exit(EXIT_FAILURE);
    }

    MM_typecode mat_code;
    if (mm_read_banner(f, &mat_code) != 0) {
        loge("Unable to process Matrix Market banner.\n");
        exit(EXIT_FAILURE);
    }

    int nrow = 0;
    int ncol = 0;
    int nnz = 0;
    if (mm_read_mtx_crd_size(f, &nrow, &ncol, &nnz) != 0) {
        loge("Unable to read the size of the sparse matrix.\n");
        exit(EXIT_FAILURE);
    }

    // Read the entries as undirected edges (smaller vertex first, 0-based),
    // dropping self-loops.
    int *coo_row = allocate_array((size_t)nnz, sizeof(int));
    int *coo_col = allocate_array((size_t)nnz, sizeof(int));
    double *coo_weight = allocate_array((size_t)nnz, sizeof(double));

    for (int i = 0; i < nnz; i++) {
        if (fscanf(f, "%d %d", &coo_row[i], &coo_col[i]) != 2) {
            loge("Unable to read entry %d of the sparse matrix.\n", i);
            exit(EXIT_FAILURE);
        }

        if (coo_row[i] == coo_col[i]) {
            read_entry_weight(f, &coo_weight[i]);
            --nnz;
            --i;
            continue;
        }

        if (coo_row[i] > coo_col[i]) {
            const int tmp = coo_row[i];
            coo_row[i] = coo_col[i];
            coo_col[i] = tmp;
        }

        --coo_row[i];
        --coo_col[i];
        read_entry_weight(f, &coo_weight[i]);
    }

    fclose(f);

    sort_rcw(coo_row, coo_col, coo_weight, nnz);

    // Store each edge in both directions, skipping duplicates. After the sort,
    // the first copy of a duplicate edge has the largest weight.
    int *row = allocate_array((size_t)nnz * 2, sizeof(int));
    graph->index = allocate_array((size_t)nnz * 2, sizeof(int));
    graph->weight = allocate_array((size_t)nnz * 2, sizeof(double));

    int j = 0;
    for (int i = 0; i < nnz; i++) {
        if (i != 0 && coo_row[i] == coo_row[i - 1] && coo_col[i] == coo_col[i - 1]) {
            continue;
        }

        row[j] = coo_row[i];
        graph->index[j] = coo_col[i];
        graph->weight[j] = coo_weight[i];
        ++j;

        row[j] = coo_col[i];
        graph->index[j] = coo_row[i];
        graph->weight[j] = coo_weight[i];
        ++j;
    }

    graph->n_edges = j;
    graph->n_nodes = nrow >= ncol ? nrow : ncol;

    if (RUN_SEQUENTIAL) {
        write_sequential_matching(row, graph);
    }

    // Group the edges by source vertex, strongest neighbor first.
    sort_rwc(row, graph->index, graph->weight, graph->n_edges);

    graph->offset = calloc((size_t)graph->n_nodes + 1, sizeof(int));
    graph->degree = calloc((size_t)graph->n_nodes, sizeof(int));

    for (int i = 0; i < graph->n_edges; i++) {
        ++graph->degree[row[i]];
    }

    exclusive_prefix_sum(graph->degree, graph->offset, graph->n_nodes);

    free(row);
    free(coo_row);
    free(coo_col);
    free(coo_weight);

    fprintf(stderr, "*** SUCCESS ***\n");
}

void read_res(const char *file_name, int n_nodes, int **res) {
    FILE *f = fopen(file_name, "r");
    if (f == NULL) {
        loge("Unable to open result file.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_nodes; i++) {
        if (fscanf(f, "%d\n", &(*res)[i]) != 1) {
            loge("Unable to read line %d of the result file.\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }

    fclose(f);
}

void graph_free(graph_data *graph) {
    free(graph->offset);
    graph->offset = NULL;
    free(graph->index);
    graph->index = NULL;
    free(graph->weight);
    graph->weight = NULL;
    free(graph->degree);
    graph->degree = NULL;
}

void check_result(graph_data graph, const int *res) {
    int *res_marker = malloc((size_t)graph.n_nodes * sizeof(int));
    for (int i = 0; i < graph.n_nodes; i++) {
        res_marker[i] = UNVISITED;
    }

    fprintf(stderr, "Start checking graph matching results...");

    for (int i = 0; i < graph.n_nodes; i++) {
        if (res_marker[i] != UNVISITED) {
            continue;
        }

        // First check that i and res[i] (res_node) are matched to each other and to
        // nothing else.
        const int res_node = res[i];
        if (res_node == UNMATCHED) {
            res_marker[i] = UNMATCHED;
            continue;
        }
        if (res_node == NO_MATCHED_NODES) {
            res_marker[i] = NO_MATCHED_NODES;
            continue;
        }
        if (res_marker[res_node] != UNVISITED) {
            fprintf(stderr,
                    "\nERROR: A node is matched to multiple nodes or the matched pair is not "
                    "legal.\n");
            fprintf(stderr, "%d -> %d\n", i, res_node);
            fprintf(stderr, "%d -> %d\n", res_marker[res_node], res_node);
            exit(EXIT_FAILURE);
        }
        if (i != res[res_node]) {
            fprintf(stderr, "\nERROR: Unmatched pair [%d, %d] is found.\n", i, res_node);
            fprintf(stderr, "%d -> %d | %d -> %d\n", i, res_node, res_node, res[res_node]);
            exit(EXIT_FAILURE);
        }

        res_marker[i] = res_node;
        res_marker[res_node] = i;

        // Then check that res[i] is a neighbor of i.
        bool is_neighbor = false;
        for (int j = graph.offset[i]; j < graph.offset[i] + graph.degree[i]; j++) {
            if (graph.index[j] == res_node) {
                is_neighbor = true;
                break;
            }
        }

        if (!is_neighbor) {
            fprintf(stderr, "\nERROR: Matched node %d is not one of the neighbor nodes of %d.\n",
                    res_node, i);
            exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "*** SUCCESS ***\n");

    free(res_marker);
}
