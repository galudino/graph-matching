///
/// \file       graph_tests.cpp
/// \brief      Unit tests for building, reading, and querying graphs.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/edge_list.h>
#include <graph_matching/graph.h>

#include "test_files.hpp"

#include <doctest/doctest.h>

#include <array>
#include <span>
#include <vector>

namespace {

///
/// \brief      Copy a vertex's neighbor list out of a graph.
///
/// \param      g           Graph to read.
/// \param      vertex      Vertex whose neighbors to copy.
///
/// \return     The neighbor IDs, strongest first.
///
std::vector<int> neighbors(const graph &g, int vertex) {
    const std::span list(graph_neighbors(&g, vertex),
                         static_cast<std::size_t>(graph_degree(&g, vertex)));
    return {list.begin(), list.end()};
}

///
/// \brief      Copy a vertex's neighbor weights out of a graph.
///
/// \param      g           Graph to read.
/// \param      vertex      Vertex whose weights to copy.
///
/// \return     The weights, in neighbor order.
///
std::vector<double> weights(const graph &g, int vertex) {
    const std::span list(graph_neighbor_weights(&g, vertex),
                         static_cast<std::size_t>(graph_degree(&g, vertex)));
    return {list.begin(), list.end()};
}

///
/// \brief      Build a graph from a few edges.
///
/// \param      g               Graph to initialize.
/// \param      vertex_count    Number of vertices.
/// \param      edges           Edges to add.
///
/// \return     The status from graph_init_from_edges().
///
status_code build(graph &g, int vertex_count, std::span<const edge> edges) {
    edge_list list;
    REQUIRE(edge_list_init(&list, static_cast<int>(edges.size())) == STATUS_OK);
    std::ranges::copy(edges, list.edges);

    const status_code status = graph_init_from_edges(&g, vertex_count, &list);
    edge_list_deinit(&list);
    return status;
}

} // namespace

TEST_CASE("graph_init_from_matrix_market builds sorted adjacency arrays from test.mtx") {
    graph g;
    REQUIRE(graph_init_from_matrix_market(&g, GRAPH_MATCHING_DATA_DIR "/test.mtx") == STATUS_OK);

    CHECK(g.vertex_count == 9);
    CHECK(g.edge_count == 12);

    const std::span offsets(g.neighbor_offsets, 10);
    CHECK(std::vector(offsets.begin(), offsets.end()) ==
          std::vector{0, 2, 5, 7, 10, 14, 17, 19, 22, 24});

    // Strongest neighbor first; equal weights by ascending vertex ID.
    CHECK(neighbors(g, 0) == std::vector{3, 1});
    CHECK(neighbors(g, 1) == std::vector{4, 2, 0});
    CHECK(neighbors(g, 2) == std::vector{5, 1});
    CHECK(neighbors(g, 3) == std::vector{4, 0, 6});
    CHECK(neighbors(g, 4) == std::vector{1, 5, 3, 7});
    CHECK(neighbors(g, 5) == std::vector{2, 4, 8});
    CHECK(neighbors(g, 6) == std::vector{3, 7});
    CHECK(neighbors(g, 7) == std::vector{4, 8, 6});
    CHECK(neighbors(g, 8) == std::vector{7, 5});

    CHECK(weights(g, 4) == std::vector{4.0, 4.0, 3.0, 3.0});
    CHECK(graph_degree(&g, 4) == 4);

    graph_deinit(&g);
    CHECK(g.neighbors == nullptr);
    CHECK(g.vertex_count == 0);
}

TEST_CASE("graph_init_from_edges drops self-loops and keeps the heaviest duplicate") {
    constexpr std::array<edge, 4> edges{{
        {.source = 0, .target = 1, .weight = 2.0},
        {.source = 1, .target = 0, .weight = 5.0},
        {.source = 2, .target = 2, .weight = 9.0},
        {.source = 2, .target = 1, .weight = 1.0},
    }};

    graph g;
    REQUIRE(build(g, 3, edges) == STATUS_OK);

    CHECK(g.edge_count == 2);
    CHECK(neighbors(g, 0) == std::vector{1});
    CHECK(weights(g, 0) == std::vector{5.0});
    CHECK(neighbors(g, 1) == std::vector{0, 2});
    CHECK(neighbors(g, 2) == std::vector{1});

    graph_deinit(&g);
}

TEST_CASE("graph_init_from_edges keeps isolated vertices and rejects out-of-range endpoints") {
    constexpr std::array<edge, 1> inside{{{.source = 0, .target = 1, .weight = 1.0}}};
    constexpr std::array<edge, 1> outside{{{.source = 0, .target = 3, .weight = 1.0}}};

    graph g;
    REQUIRE(build(g, 4, inside) == STATUS_OK);
    CHECK(g.vertex_count == 4);
    CHECK(graph_degree(&g, 3) == 0);
    graph_deinit(&g);

    CHECK(build(g, 3, outside) == STATUS_INVALID_ARGUMENT);
    CHECK(g.neighbors == nullptr);
    graph_deinit(&g);
}

TEST_CASE("graph_init_from_matrix_market reads pattern matrices with weight 1") {
    const graph_matching::testing::temporary_file file(
        "graph_matching_pattern.mtx",
        "%%MatrixMarket matrix coordinate pattern symmetric\n3 3 2\n2 1\n3 2\n");

    graph g;
    REQUIRE(graph_init_from_matrix_market(&g, file.c_str()) == STATUS_OK);
    CHECK(g.edge_count == 2);
    CHECK(weights(g, 1) == std::vector{1.0, 1.0});
    graph_deinit(&g);
}

TEST_CASE("graph_init_from_matrix_market reports unreadable and malformed files") {
    graph g;
    CHECK(graph_init_from_matrix_market(&g, "/nonexistent/graph.mtx") == STATUS_FILE_ERROR);

    const graph_matching::testing::temporary_file no_banner("graph_matching_no_banner.mtx",
                                                            "3 3 1\n2 1 1\n");
    CHECK(graph_init_from_matrix_market(&g, no_banner.c_str()) == STATUS_PARSE_ERROR);

    const graph_matching::testing::temporary_file out_of_range(
        "graph_matching_out_of_range.mtx",
        "%%MatrixMarket matrix coordinate real general\n3 3 1\n4 1 1\n");
    CHECK(graph_init_from_matrix_market(&g, out_of_range.c_str()) == STATUS_PARSE_ERROR);

    const graph_matching::testing::temporary_file truncated(
        "graph_matching_truncated.mtx",
        "%%MatrixMarket matrix coordinate real general\n3 3 2\n2 1 1\n");
    CHECK(graph_init_from_matrix_market(&g, truncated.c_str()) == STATUS_PARSE_ERROR);

    CHECK(g.neighbors == nullptr);
    graph_deinit(&g);
}
