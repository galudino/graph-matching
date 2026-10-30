///
/// \file       matrix_generator.cpp
/// \brief      Implementation of the random test matrix generator.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/matrix_generator.hpp>

#include <climits>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <print>
#include <random>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

namespace graph_matching {

namespace {

/// graph_read_matrix_market() stores each edge twice in int-indexed arrays.
constexpr std::int64_t max_entries = INT_MAX / 2;

} // namespace

std::vector<edge> generate_edges(const generator_options &options) {
    if (options.vertices < 2) {
        throw std::invalid_argument("vertices must be at least 2");
    }
    if (options.average_degree < 1) {
        throw std::invalid_argument("average degree must be at least 1");
    }
    if (options.max_weight < 1) {
        throw std::invalid_argument("max weight must be at least 1");
    }

    const auto entries = static_cast<std::int64_t>(options.vertices) * options.average_degree / 2;
    if (entries > max_entries) {
        throw std::invalid_argument(
            "too many entries to read back as a graph; lower vertices or degree");
    }

    std::mt19937_64 engine(options.seed);
    std::uniform_int_distribution<int> first_vertex(0, options.vertices - 1);
    std::uniform_int_distribution<int> second_vertex(0, options.vertices - 2);
    std::uniform_int_distribution<int> weight(1, options.max_weight);

    std::vector<edge> edges;
    edges.reserve(static_cast<std::size_t>(entries));

    for (std::int64_t i = 0; i < entries; ++i) {
        const int u = first_vertex(engine);

        // Draw from the other vertices - 1 vertices, then skip over u, so the
        // edge is never a self-loop.
        int v = second_vertex(engine);
        if (v >= u) {
            ++v;
        }

        const auto [smaller, larger] = std::minmax(u, v);
        edges.push_back({.row = larger, .col = smaller, .weight = weight(engine)});
    }

    return edges;
}

void write_matrix_market(std::ostream &out, const generator_options &options,
                         std::span<const edge> edges) {
    std::print(out, "%%MatrixMarket matrix coordinate real symmetric\n");
    std::print(out, "% Random graph: average degree {}, weights 1 to {}, seed {}\n",
               options.average_degree, options.max_weight, options.seed);
    std::print(out, "{} {} {}\n", options.vertices, options.vertices, edges.size());

    // Matrix Market indices are 1-based.
    for (const edge &e : edges) {
        std::print(out, "{} {} {}\n", e.row + 1, e.col + 1, e.weight);
    }
}

} // namespace graph_matching
