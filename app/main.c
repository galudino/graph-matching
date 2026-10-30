///
/// \file       main.c
/// \brief      Entry point for the graph matching command-line application.
///
/// \details    Reads a Matrix Market graph, matches it with one-way or N-way
///             handshaking (or the greedy baseline), writes the result, and
///             checks it.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/graph.h>
#include <graph_matching/handshake.h>
#include <graph_matching/matching.h>
#include <graph_matching/status_code.h>
#include <graph_matching/stopwatch.h>

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///
/// \brief      Print command-line usage to stderr.
///
/// \param      program     Program name to show, usually argv[0].
///
static void print_usage(const char *program) {
    fprintf(stderr,
            "Usage: %s <input.mtx> <output> one-way <threads>\n"
            "       %s <input.mtx> <output> n-way <hands> <threads>\n"
            "       %s <input.mtx> <output> greedy\n"
            "\n"
            "  one-way   Each vertex extends one hand per pass.\n"
            "  n-way     Each vertex extends <hands> hands per pass (at least 2).\n"
            "  greedy    Sequential baseline: heaviest edges first.\n"
            "\n"
            "The output has one line per vertex: its partner, or -2 if it has none.\n",
            program, program, program);
}

///
/// \brief      Parse a whole string as an int of at least a minimum value.
///
/// \param      text        Text to parse.
/// \param      minimum     Smallest accepted value.
/// \param      value       Receives the value.
///
/// \return     True if text is entirely an int no smaller than minimum.
///
static bool parse_int(const char *text, int minimum, int *value) {
    char *end = NULL;
    errno = 0;
    const long parsed = strtol(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || parsed < minimum || parsed > INT_MAX) {
        return false;
    }

    *value = (int)parsed;
    return true;
}

///
/// \brief      Matching method selected on the command line.
///
typedef enum matching_method {
    METHOD_HANDSHAKE, ///< One-way or N-way handshaking.
    METHOD_GREEDY,    ///< Sequential greedy baseline.
} matching_method;

///
/// \brief      Parsed command-line arguments.
///
typedef struct arguments {
    const char *input_path;    ///< Matrix Market file to read.
    const char *output_path;   ///< File to write the matching to.
    matching_method method;    ///< Matching method.
    handshake_options options; ///< Thread and hand counts, for handshaking.
} arguments;

///
/// \brief      Parse the command line.
///
/// \param      argc        Number of arguments.
/// \param      argv        Arguments.
/// \param      result      Receives the parsed arguments.
///
/// \return     True if the arguments are valid.
///
static bool parse_arguments(int argc, const char *argv[], arguments *result) {
    if (argc < 4) {
        return false;
    }

    result->input_path = argv[1];
    result->output_path = argv[2];
    const char *method = argv[3];

    if (strcmp(method, "one-way") == 0 && argc == 5) {
        result->method = METHOD_HANDSHAKE;
        result->options.hand_count = 1;
        return parse_int(argv[4], 1, &result->options.thread_count);
    }

    if (strcmp(method, "n-way") == 0 && argc == 6) {
        result->method = METHOD_HANDSHAKE;
        return parse_int(argv[4], 2, &result->options.hand_count) &&
               parse_int(argv[5], 1, &result->options.thread_count);
    }

    if (strcmp(method, "greedy") == 0 && argc == 4) {
        result->method = METHOD_GREEDY;
        return true;
    }

    return false;
}

///
/// \brief      Application entry point.
///
/// \param      argc        Number of command-line arguments.
/// \param      argv        Command-line arguments.
///
/// \return     EXIT_SUCCESS if the matching was written and is valid;
///             EXIT_FAILURE otherwise.
///
int main(int argc, const char *argv[]) {
    arguments args = {0};
    if (!parse_arguments(argc, argv, &args)) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    stopwatch timer;
    stopwatch_start(&timer);

    graph input;
    status_code status = graph_read_matrix_market(&input, args.input_path);
    if (status != STATUS_OK) {
        fprintf(stderr, "Could not read %s: %s.\n", args.input_path, status_code_string(status));
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Read %s: %d vertices, %d edges (%.2f ms).\n", args.input_path,
            input.vertex_count, input.edge_count, stopwatch_elapsed_ms(&timer));

    matching result;
    status = matching_init(&result, input.vertex_count);

    if (status == STATUS_OK && args.method == METHOD_GREEDY) {
        stopwatch_start(&timer);
        status = matching_run_greedy(&result, &input);
        fprintf(stderr, "Greedy matching: %.2f ms.\n", stopwatch_elapsed_ms(&timer));
    } else if (status == STATUS_OK) {
        handshake_statistics statistics = {0};
        status = handshake_match(&result, &input, &args.options, &statistics);

        if (args.options.hand_count == 1) {
            fprintf(stderr, "One-way matching");
        } else {
            fprintf(stderr, "%d-way matching", args.options.hand_count);
        }
        fprintf(stderr, " on %d threads: %ld passes, %.2f ms.\n", args.options.thread_count,
                statistics.pass_count, statistics.elapsed_ms);
    }

    if (status == STATUS_OK) {
        status = matching_write(&result, args.output_path);
        if (status != STATUS_OK) {
            fprintf(stderr, "Could not write %s: %s.\n", args.output_path,
                    status_code_string(status));
        }
    } else {
        fprintf(stderr, "Matching failed: %s.\n", status_code_string(status));
    }

    if (status == STATUS_OK) {
        status = matching_validate(&result, &input);
        if (status == STATUS_OK) {
            fprintf(stderr, "Valid %s matching: %d pairs, total weight %g. Written to %s.\n",
                    matching_is_maximal(&result, &input) ? "maximal" : "non-maximal",
                    matching_pair_count(&result), matching_weight(&result, &input),
                    args.output_path);
        } else {
            fprintf(stderr, "Invalid matching written to %s.\n", args.output_path);
        }
    }

    matching_destroy(&result);
    graph_destroy(&input);

    return status == STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
