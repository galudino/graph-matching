///
/// \file       utils.h
/// \brief      Diagnostics, timing, prefix sums, and graph and result I/O.
///
/// \details    Adapted from course-provided starter code.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#ifndef GRAPH_MATCHING_UTILS_H
#define GRAPH_MATCHING_UTILS_H

#include <graph_matching/graph_data.h>

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Print a debug message through err(), tagged with the calling function and line.
#define logd(...) err("debug", __func__, __LINE__, __VA_ARGS__)

/// Report an error through err(), tagged with the calling function and line.
#define loge(...) err("error", __func__, __LINE__, __VA_ARGS__)

/// Report a warning through err(), tagged with the calling function and line.
#define logw(...) err("warning", __func__, __LINE__, __VA_ARGS__)

/// Print an info message through err(), tagged with the calling function and line.
#define logi(...) err("info", __func__, __LINE__, __VA_ARGS__)

/// When nonzero, read_mm() writes a sequential greedy matching to seq.out and exits.
#define RUN_SEQUENTIAL 0

/// Let GCC and Clang check printf-style arguments against the format string.
#if defined(__GNUC__)
#define GRAPH_MATCHING_PRINTF_FORMAT(format_index, first_arg)                                      \
    __attribute__((format(printf, format_index, first_arg)))
#else
#define GRAPH_MATCHING_PRINTF_FORMAT(format_index, first_arg)
#endif

///
/// \brief      Print a tagged diagnostic message to stderr.
///
/// \details    Messages longer than 1023 characters are truncated. Use the
///             logd (debug), loge (error), logw (warning), and logi (info) macros
///             instead of calling this directly.
///
/// \param      level       Severity tag, such as "error".
/// \param      func        Name of the reporting function.
/// \param      line        Source line of the report.
/// \param      fmt         printf-style format string.
///
/// \return     The number of characters written, or a negative value on error.
///
int err(const char *level, const char *func, int line, const char *fmt, ...)
    GRAPH_MATCHING_PRINTF_FORMAT(4, 5);

///
/// \brief      Record the current time from a monotonic clock.
///
/// \param      time        Receives the current time.
///
void set_time(struct timespec *time);

///
/// \brief      Compute the time between two set_time() readings.
///
/// \param      start_time  Earlier reading.
/// \param      end_time    Later reading.
///
/// \return     Elapsed time in milliseconds.
///
double get_time(const struct timespec *start_time, const struct timespec *end_time);

///
/// \brief      Swap two int array pointers.
///
/// \param      array_a     First pointer.
/// \param      array_b     Second pointer.
///
void swap_int_array(int **array_a, int **array_b);

///
/// \brief      Compute the exclusive prefix sum of an array.
///
/// \details    sum[0] is 0 and sum[i] is num[0] + ... + num[i - 1], so
///             sum[size] holds the total.
///
/// \param      num         Input values; size entries.
/// \param      sum         Output; size + 1 entries.
/// \param      size        Number of input values.
///
void exclusive_prefix_sum(const int *num, int *sum, int size);

///
/// \brief      Compute the inclusive prefix sum of an array.
///
/// \details    sum[i] is num[0] + ... + num[i].
///
/// \param      num         Input values; size entries (at least one).
/// \param      sum         Output; size entries.
/// \param      size        Number of input values.
///
void inclusive_prefix_sum(const int *num, int *sum, int size);

///
/// \brief      Print every vertex's degree and weighted neighbor list to stderr.
///
/// \param      graph       Graph to print.
///
void print_graph(graph_data graph);

///
/// \brief      Print a matching result to stderr in human-readable form.
///
/// \param      res         Matching result; n_nodes entries.
/// \param      n_nodes     Number of vertices.
///
void print_result(const int *res, int n_nodes);

///
/// \brief      Print command-line usage to stderr.
///
/// \param      program     Program name to show, usually argv[0].
///
void print_usage(const char *program);

///
/// \brief      Read a Matrix Market file into an undirected weighted graph.
///
/// \details    Each off-diagonal entry (i, j) becomes an undirected edge
///             between vertices i - 1 and j - 1. Self-loops and duplicate
///             edges are dropped, and pattern entries get weight 1. Neighbor
///             lists are sorted as described for graph_data. Exits the
///             program if the file cannot be read.
///
/// \param      file_name   Path to the .mtx file.
/// \param      graph       Receives the graph; release it with graph_free().
///
void read_mm(const char *file_name, graph_data *graph);

///
/// \brief      Release the arrays allocated by read_mm().
///
/// \param      graph       Graph to release; its pointers are set to NULL.
///
void graph_free(graph_data *graph);

///
/// \brief      Write a matching result, one res[] value per line.
///
/// \details    Exits the program if the file cannot be opened.
///
/// \param      file_name   Path of the output file.
/// \param      res         Matching result; n_nodes entries.
/// \param      n_nodes     Number of vertices.
///
void write_match_result(const char *file_name, const int *res, int n_nodes);

///
/// \brief      Read a matching result written by write_match_result().
///
/// \details    Exits the program if the file cannot be read.
///
/// \param      file_name   Path of the result file.
/// \param      n_nodes     Number of vertices.
/// \param      res         Points to an array of n_nodes entries to fill.
///
void read_res(const char *file_name, int n_nodes, int **res);

///
/// \brief      Verify that a result is a valid matching of a graph.
///
/// \details    Checks that matches are mutual, that no vertex is matched
///             twice, and that every matched pair is an edge of the graph.
///             Prints the first problem found and exits the program.
///
/// \param      graph       Graph that was matched.
/// \param      res         Matching result; graph.n_nodes entries.
///
void check_result(graph_data graph, const int *res);

#ifdef __cplusplus
}
#endif

#endif // GRAPH_MATCHING_UTILS_H
