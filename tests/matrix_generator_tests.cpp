///
/// \file       matrix_generator_tests.cpp
/// \brief      Unit tests for the random test matrix generator.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/graph_data.h>
#include <graph_matching/matrix_generator.hpp>
#include <graph_matching/utils.h>

#include <doctest/doctest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <span>
#include <stdexcept>
#include <vector>

namespace {

///
/// \brief      Compare edges field by field.
///
/// \param      a           First edge list.
/// \param      b           Second edge list.
///
/// \return     True if both lists hold the same edges in the same order.
///
bool same_edges(std::span<const graph_matching::edge> a, std::span<const graph_matching::edge> b) {
    return std::ranges::equal(a, b, [](const auto &x, const auto &y) {
        return x.row == y.row && x.col == y.col && x.weight == y.weight;
    });
}

} // namespace

TEST_CASE("generate_edges draws vertices * average_degree / 2 valid edges") {
    const graph_matching::generator_options options{
        .vertices = 500, .average_degree = 7, .max_weight = 3, .seed = 42};

    const auto edges = graph_matching::generate_edges(options);

    CHECK(edges.size() == 500 * 7 / 2);
    CHECK(std::ranges::all_of(edges, [](const auto &e) {
        return e.col >= 0 && e.row > e.col && e.row < 500 && e.weight >= 1 && e.weight <= 3;
    }));
}

TEST_CASE("generate_edges is reproducible for a seed") {
    graph_matching::generator_options options{.vertices = 200, .average_degree = 4};

    const auto first = graph_matching::generate_edges(options);
    const auto again = graph_matching::generate_edges(options);
    options.seed = 2;
    const auto other = graph_matching::generate_edges(options);

    CHECK(same_edges(first, again));
    CHECK_FALSE(same_edges(first, other));
}

TEST_CASE("generate_edges rejects out-of-range options") {
    // Only the exception matters, so discard the (never returned) result.
    const auto generate = [](const graph_matching::generator_options &options) {
        static_cast<void>(graph_matching::generate_edges(options));
    };

    CHECK_THROWS_AS(generate({.vertices = 1}), std::invalid_argument);
    CHECK_THROWS_AS(generate({.average_degree = 0}), std::invalid_argument);
    CHECK_THROWS_AS(generate({.max_weight = 0}), std::invalid_argument);
    CHECK_THROWS_AS(generate({.vertices = 1'000'000'000, .average_degree = 100}),
                    std::invalid_argument);
}

TEST_CASE("generated matrices read back with read_mm") {
    const graph_matching::generator_options options{.vertices = 300, .average_degree = 6};
    const auto edges = graph_matching::generate_edges(options);

    const auto path = std::filesystem::temp_directory_path() / "graph_matching_generated_test.mtx";
    {
        std::ofstream file(path);
        graph_matching::write_matrix_market(file, options, edges);
    }

    graph_data graph{};
    read_mm(path.c_str(), &graph);

    CHECK(graph.n_nodes == 300);

    // Each distinct edge is stored in both directions; repeated pairs collapse.
    CHECK(graph.n_edges % 2 == 0);
    CHECK(graph.n_edges <= 2 * static_cast<int>(edges.size()));
    CHECK(graph.n_edges > 2 * static_cast<int>(edges.size()) * 9 / 10);

    // Every stored entry is in exactly one vertex's neighbor list.
    const std::span degree(graph.degree, static_cast<std::size_t>(graph.n_nodes));
    int total_degree = 0;
    for (const int d : degree) {
        total_degree += d;
    }
    CHECK(total_degree == graph.n_edges);

    graph_free(&graph);
    std::filesystem::remove(path);
}
