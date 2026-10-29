///
/// \file       read_mm_tests.cpp
/// \brief      Unit tests for reading data/test.mtx and checking results.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/graph_data.h>
#include <graph_matching/match.h>
#include <graph_matching/utils.h>

#include <doctest/doctest.h>

#include <array>
#include <span>
#include <vector>

namespace {

///
/// \brief      Read data/test.mtx, a 9-vertex example matrix.
///
/// \return     The graph; release it with graph_free().
///
graph_data read_test_matrix() {
    graph_data graph{};
    read_mm(GRAPH_MATCHING_DATA_DIR "/test.mtx", &graph);
    return graph;
}

///
/// \brief      Copy a vertex's neighbor list out of the graph.
///
/// \param      graph       Graph to read.
/// \param      vertex      Vertex whose neighbors to copy.
///
/// \return     The neighbor IDs, strongest first.
///
std::vector<int> neighbors(const graph_data &graph, int vertex) {
    const std::span index(graph.index, static_cast<std::size_t>(graph.n_edges));
    const auto first = index.begin() + graph.offset[vertex];
    return {first, first + graph.degree[vertex]};
}

} // namespace

TEST_CASE("read_mm builds sorted adjacency arrays from test.mtx") {
    graph_data graph = read_test_matrix();

    CHECK(graph.n_nodes == 9);
    CHECK(graph.n_edges == 24);

    const std::span offset(graph.offset, 10);
    CHECK(std::vector(offset.begin(), offset.end()) ==
          std::vector{0, 2, 5, 7, 10, 14, 17, 19, 22, 24});

    // Strongest neighbor first; equal weights by ascending vertex ID.
    CHECK(neighbors(graph, 0) == std::vector{3, 1});
    CHECK(neighbors(graph, 1) == std::vector{4, 2, 0});
    CHECK(neighbors(graph, 2) == std::vector{5, 1});
    CHECK(neighbors(graph, 3) == std::vector{4, 0, 6});
    CHECK(neighbors(graph, 4) == std::vector{1, 5, 3, 7});
    CHECK(neighbors(graph, 5) == std::vector{2, 4, 8});
    CHECK(neighbors(graph, 6) == std::vector{3, 7});
    CHECK(neighbors(graph, 7) == std::vector{4, 8, 6});
    CHECK(neighbors(graph, 8) == std::vector{7, 5});

    graph_free(&graph);
    CHECK(graph.index == nullptr);
}

TEST_CASE("check_result accepts a valid matching of test.mtx") {
    graph_data graph = read_test_matrix();

    // Pairs (0, 3), (1, 4), (2, 5), and (6, 7) are all edges; 8 is left over.
    const std::array res{3, 4, 5, 0, 1, 2, 7, 6, NO_MATCHED_NODES};

    // check_result() exits the process on an invalid matching.
    check_result(graph, res.data());

    graph_free(&graph);
}
