///
/// \file       main.cpp
/// \brief      Entry point for the C++ graph matching command-line application.
///
/// \details    Takes the same arguments as the C program, graph_matching_c.
///             The matching itself is not implemented yet.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <print>
#include <span>
#include <string_view>

namespace {

///
/// \brief      Print command-line usage.
///
/// \param      stream      Stream to write to: stdout for --help, else stderr.
/// \param      program     Program name to show, usually argv[0].
///
void print_usage(std::FILE *stream, std::string_view program) {
    std::println(stream,
                 "Usage: {0} <input.mtx> <output> one-way <threads>\n"
                 "       {0} <input.mtx> <output> n-way <hands> <threads>\n"
                 "       {0} <input.mtx> <output> greedy\n"
                 "\n"
                 "  one-way   Each vertex extends one hand per pass.\n"
                 "  n-way     Each vertex extends <hands> hands per pass (at least 2).\n"
                 "  greedy    Sequential baseline: heaviest edges first.\n"
                 "\n"
                 "The output has one line per vertex: its partner, or -2 if it has none.",
                 program);
}

} // namespace

///
/// \brief      Application entry point.
///
/// \param      argc        Number of command-line arguments.
/// \param      argv        Command-line arguments.
///
/// \return     EXIT_SUCCESS for --help; EXIT_FAILURE otherwise, until
///             matching is implemented.
///
int main(int argc, char *argv[]) try {
    const std::span arguments(argv, static_cast<std::size_t>(argc));
    const std::string_view program = arguments.empty() ? "graph_matching_cpp" : arguments[0];

    if (arguments.size() == 2 && std::string_view(arguments[1]) == "--help") {
        print_usage(stdout, program);
        return EXIT_SUCCESS;
    }

    print_usage(stderr, program);
    std::println(stderr, "\nMatching is not implemented yet.");

    return EXIT_FAILURE;
} catch (const std::exception &error) {
    // C stdio cannot throw, so reporting the failure cannot fail the same way.
    std::fputs(error.what(), stderr);
    std::fputc('\n', stderr);

    return EXIT_FAILURE;
}
