///
/// \file       generate_matrix.cpp
/// \brief      Command-line tool that writes a random test matrix.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/matrix_generator.hpp>

#include <charconv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <optional>
#include <print>
#include <span>
#include <string_view>
#include <system_error>

namespace {

///
/// \brief      Print command-line usage to stderr.
///
/// \param      program     Program name to show, usually argv[0].
///
void print_usage(std::string_view program) {
    std::println(stderr,
                 "Usage: {} <output.mtx> <vertices> <average degree> [max weight] [seed]\n"
                 "       e.g. {} data/generated/small.mtx 1000 8\n"
                 "       [max weight]: weights are 1 to this value (default 10)\n"
                 "       [seed]: random seed (default 1)",
                 program, program);
}

///
/// \brief      Parse a whole string as a number.
///
/// \tparam     Number      Integer type to parse.
///
/// \param      text        Text to parse.
///
/// \return     The number, or std::nullopt if text is not entirely a number.
///
template <typename Number>
std::optional<Number> parse(std::string_view text) {
    Number value{};
    const auto [end, ec] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (ec != std::errc{} || end != text.data() + text.size()) {
        return std::nullopt;
    }
    return value;
}

} // namespace

///
/// \brief      Application entry point.
///
/// \param      argc        Number of command-line arguments.
/// \param      argv        Command-line arguments.
///
/// \return     EXIT_SUCCESS after writing the matrix; EXIT_FAILURE on bad
///             arguments or an error.
///
int main(int argc, char *argv[]) try {
    const std::span args(argv, static_cast<std::size_t>(argc));

    if (args.size() < 4 || args.size() > 6) {
        print_usage(args[0]);
        return EXIT_FAILURE;
    }

    graph_matching::generator_options options;
    const std::filesystem::path output = args[1];

    const auto vertices = parse<int>(args[2]);
    const auto average_degree = parse<int>(args[3]);
    const auto max_weight = args.size() > 4 ? parse<int>(args[4]) : options.max_weight;
    const auto seed = args.size() > 5 ? parse<std::uint64_t>(args[5]) : options.seed;

    if (!vertices || !average_degree || !max_weight || !seed) {
        print_usage(args[0]);
        return EXIT_FAILURE;
    }

    options.vertices = *vertices;
    options.average_degree = *average_degree;
    options.max_weight = *max_weight;
    options.seed = *seed;

    const auto edges = graph_matching::generate_edges(options);

    if (output.has_parent_path()) {
        std::filesystem::create_directories(output.parent_path());
    }

    std::ofstream file(output);
    if (!file) {
        std::println(stderr, "Could not open {} for writing.", output.string());
        return EXIT_FAILURE;
    }

    graph_matching::write_matrix_market(file, options, edges);

    file.close();
    if (!file) {
        std::println(stderr, "Could not finish writing {}.", output.string());
        return EXIT_FAILURE;
    }

    std::println(stderr, "Wrote {} vertices and {} entries to {}.", options.vertices, edges.size(),
                 output.string());

    return EXIT_SUCCESS;
} catch (const std::exception &error) {
    // C stdio cannot throw, so reporting the failure cannot fail the same way.
    std::fputs(error.what(), stderr);
    std::fputc('\n', stderr);

    return EXIT_FAILURE;
}
