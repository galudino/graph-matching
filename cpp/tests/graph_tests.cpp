///
/// \file       graph_tests.cpp
/// \brief      Unit tests for building, reading, and querying graphs.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching_cpp/edge.hpp>
#include <graph_matching_cpp/graph.hpp>

#include "temporary_file.hpp"

#include <doctest/doctest.h>

#include <array>
#include <filesystem>
#include <span>
#include <vector>

using graph_matching::edge;
using graph_matching::graph;
using graph_matching::vertex_id;
using graph_matching::testing::temporary_file;

namespace {

///
/// \brief      Copy a vertex's neighbor list out of a graph.
///
/// \param      g           Graph to read.
/// \param      vertex      Vertex whose neighbors to copy.
///
/// \return     The neighbor IDs, strongest first.
///
std::vector<vertex_id> neighbors(const graph &g, vertex_id vertex) {
    const auto list = g.neighbors(vertex);
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
std::vector<double> weights(const graph &g, vertex_id vertex) {
    const auto list = g.neighbor_weights(vertex);
    return {list.begin(), list.end()};
}

///
/// \brief      Read a Matrix Market file from text.
///
/// \param      name        Temporary file name.
/// \param      contents    File contents.
///
/// \return     The result of graph::from_matrix_market().
///
std::expected<graph, graph::err> read(std::string_view name, std::string_view contents) {
    const temporary_file file(name, contents);
    return graph::from_matrix_market(file.path());
}

} // namespace

TEST_CASE("from_matrix_market builds sorted adjacency arrays from test.mtx") {
    const auto g = graph::from_matrix_market(GRAPH_MATCHING_CPP_DATA_DIR "/test.mtx");
    REQUIRE(g.has_value());

    CHECK(g->vertex_count() == 9);
    CHECK(g->edge_count() == 12);

    // Strongest neighbor first; equal weights by ascending vertex ID.
    CHECK(neighbors(*g, 0) == std::vector<vertex_id>{3, 1});
    CHECK(neighbors(*g, 1) == std::vector<vertex_id>{4, 2, 0});
    CHECK(neighbors(*g, 2) == std::vector<vertex_id>{5, 1});
    CHECK(neighbors(*g, 3) == std::vector<vertex_id>{4, 0, 6});
    CHECK(neighbors(*g, 4) == std::vector<vertex_id>{1, 5, 3, 7});
    CHECK(neighbors(*g, 5) == std::vector<vertex_id>{2, 4, 8});
    CHECK(neighbors(*g, 6) == std::vector<vertex_id>{3, 7});
    CHECK(neighbors(*g, 7) == std::vector<vertex_id>{4, 8, 6});
    CHECK(neighbors(*g, 8) == std::vector<vertex_id>{7, 5});

    CHECK(weights(*g, 4) == std::vector{4.0, 4.0, 3.0, 3.0});
    CHECK(g->degree(4) == 4);
}

TEST_CASE("from_edges drops self-loops and keeps the heaviest duplicate") {
    constexpr std::array<edge, 4> edges{{
        {.source = 0, .target = 1, .weight = 2.0},
        {.source = 1, .target = 0, .weight = 5.0},
        {.source = 2, .target = 2, .weight = 9.0},
        {.source = 2, .target = 1, .weight = 1.0},
    }};

    const auto g = graph::from_edges(3, edges);
    REQUIRE(g.has_value());

    CHECK(g->edge_count() == 2);
    CHECK(neighbors(*g, 0) == std::vector<vertex_id>{1});
    CHECK(weights(*g, 0) == std::vector{5.0});
    CHECK(neighbors(*g, 1) == std::vector<vertex_id>{0, 2});
    CHECK(neighbors(*g, 2) == std::vector<vertex_id>{1});
}

TEST_CASE("from_edges keeps isolated vertices and rejects out-of-range arguments") {
    constexpr std::array<edge, 1> inside{{{.source = 0, .target = 1, .weight = 1.0}}};
    constexpr std::array<edge, 1> outside{{{.source = 0, .target = 3, .weight = 1.0}}};

    const auto g = graph::from_edges(4, inside);
    REQUIRE(g.has_value());
    CHECK(g->vertex_count() == 4);
    CHECK(g->degree(3) == 0);
    CHECK(g->neighbors(3).empty());

    CHECK(graph::from_edges(3, outside).error() == graph::err::invalid_argument);
    CHECK(graph::from_edges(-1, {}).error() == graph::err::invalid_argument);

    const auto empty = graph::from_edges(0, {});
    REQUIRE(empty.has_value());
    CHECK(empty->vertex_count() == 0);
    CHECK(empty->edge_count() == 0);
}

TEST_CASE("from_matrix_market reads pattern matrices with weight 1") {
    const auto g = read("graph_matching_cpp_pattern.mtx",
                        "%%MatrixMarket matrix coordinate pattern symmetric\n3 3 2\n2 1\n3 2\n");
    REQUIRE(g.has_value());

    CHECK(g->edge_count() == 2);
    CHECK(weights(*g, 1) == std::vector{1.0, 1.0});
}

TEST_CASE("from_matrix_market skips comments and accepts any case and line ending") {
    const auto g = read("graph_matching_cpp_comments.mtx",
                        "%%matrixmarket MATRIX Coordinate Integer General\r\n"
                        "% a comment\r\n"
                        "\r\n"
                        "3 3 2\r\n"
                        "2 1 7\r\n"
                        "3 2 +2.5e0\r\n");
    REQUIRE(g.has_value());

    CHECK(weights(*g, 1) == std::vector{7.0, 2.5});
}

TEST_CASE("from_matrix_market reports unreadable and malformed files") {
    CHECK(graph::from_matrix_market("/nonexistent/graph.mtx").error() == graph::err::file_error);

    const auto parse_error = [](std::string_view contents) {
        const auto g = read("graph_matching_cpp_malformed.mtx", contents);
        return !g.has_value() && g.error() == graph::err::parse_error;
    };

    CHECK(parse_error(""));
    CHECK(parse_error("3 3 1\n2 1 1\n"));
    CHECK(parse_error("%%MatrixMarket matrix array real general\n3 3\n"));
    CHECK(parse_error("%%MatrixMarket matrix coordinate complex general\n3 3 1\n2 1 1 0\n"));
    CHECK(parse_error("%%MatrixMarket matrix coordinate real general\n3 3 1\n4 1 1\n"));
    CHECK(parse_error("%%MatrixMarket matrix coordinate real general\n3 3 2\n2 1 1\n"));
    CHECK(parse_error("%%MatrixMarket matrix coordinate real general\n3 3 1\n2 x 1\n"));
    CHECK(parse_error("%%MatrixMarket matrix coordinate real general\n3 3 1\n2 1 1 9\n"));
    CHECK(parse_error("%%MatrixMarket matrix coordinate real general\n3 -3 1\n"));
}

TEST_CASE("to_string describes every error") {
    CHECK(to_string(graph::err::invalid_argument) == "invalid argument");
    CHECK(to_string(graph::err::file_error) == "file could not be opened or read");
    CHECK(to_string(graph::err::parse_error) == "malformed Matrix Market file");
}
