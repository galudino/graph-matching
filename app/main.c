///
/// \file       main.c
/// \brief      Entry point for the graph matching command-line application.
///
/// \details    Reads a Matrix Market graph, runs one-way or N-way handshaking
///             matching, and writes the result. Adapted from main.c in the
///             CS 314 (Rutgers, Fall 2018) Project 3 code package.
///
/// \author     Yanhao Chen (original)
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/graph_data.h>
#include <graph_matching/match.h>
#include <graph_matching/utils.h>

#include <stdlib.h>
#include <string.h>

///
/// \brief      Matching algorithm selected on the command line.
///
typedef enum method_type {
    ONEWAY = 1, ///< One-way handshaking ("oneWay").
    NWAYS = 2,  ///< N-way handshaking ("nWays").
} method_type;

///
/// \brief      Application entry point.
///
/// \details    Usage: input.mtx output oneWay threads, or
///             input.mtx output nWays hands threads.
///
/// \param      argc        Number of command-line arguments.
/// \param      argv        Command-line arguments.
///
/// \return     EXIT_SUCCESS after writing the result; EXIT_FAILURE on bad arguments.
///
int main(int argc, const char *argv[]) {
    if (argc != 5 && argc != 6) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    int arg = 1;
    const char *input_file = argv[arg++];
    const char *output_file = argv[arg++];

    method_type method = ONEWAY;
    int n_ways = 1;

    if (strcmp(argv[arg], "oneWay") == 0) {
        if (argc != 5) {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
        ++arg;
        method = ONEWAY;
    } else if (strcmp(argv[arg], "nWays") == 0) {
        if (argc != 6) {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
        ++arg;
        method = NWAYS;
        n_ways = atoi(argv[arg++]);
        if (n_ways < 2) {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    } else {
        loge("Invalid matching algorithm (method) provided.\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const int n_threads = atoi(argv[arg]);

    // Read the graph; its neighbor lists come back sorted, strongest first.
    graph_data graph;
    read_mm(input_file, &graph);

    int *res = malloc((size_t)graph.n_nodes * sizeof(int));
    for (int i = 0; i < graph.n_nodes; i++) {
        res[i] = UNMATCHED;
    }

    if (method == ONEWAY) {
        one_way_handshake_wrapper(n_threads, graph, &res);
    } else {
        n_ways_handshake_wrapper(n_threads, n_ways, graph, &res);
    }

    write_match_result(output_file, res, graph.n_nodes);

    graph_free(&graph);
    free(res);

    return EXIT_SUCCESS;
}
