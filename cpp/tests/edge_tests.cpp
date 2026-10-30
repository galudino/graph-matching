///
/// \file       edge_tests.cpp
/// \brief      Unit tests for sorting edges.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching_cpp/edge.hpp>

#include <doctest/doctest.h>

#include <vector>

using graph_matching::edge;
using graph_matching::edge_order;
using graph_matching::sort_edges;

namespace {

///
/// \brief      Edges with ties in every key, in no particular order.
///
const std::vector<edge> unsorted_edges{
    {.source = 1, .target = 2, .weight = 1.0}, {.source = 0, .target = 3, .weight = 2.0},
    {.source = 1, .target = 0, .weight = 5.0}, {.source = 0, .target = 1, .weight = 2.0},
    {.source = 1, .target = 2, .weight = 3.0}, {.source = 0, .target = 3, .weight = 4.0},
};

///
/// \brief      Sort a copy of unsorted_edges.
///
/// \param      order       Key order.
///
/// \return     The sorted edges.
///
std::vector<edge> sorted(edge_order order) {
    std::vector<edge> edges = unsorted_edges;
    sort_edges(edges, order);
    return edges;
}

} // namespace

TEST_CASE("sort_edges orders by source, target, then heaviest first") {
    CHECK(sorted(edge_order::source_target_weight) ==
          std::vector<edge>{{0, 1, 2}, {0, 3, 4}, {0, 3, 2}, {1, 0, 5}, {1, 2, 3}, {1, 2, 1}});
}

TEST_CASE("sort_edges orders by source, heaviest first, then target") {
    CHECK(sorted(edge_order::source_weight_target) ==
          std::vector<edge>{{0, 3, 4}, {0, 1, 2}, {0, 3, 2}, {1, 0, 5}, {1, 2, 3}, {1, 2, 1}});
}

TEST_CASE("sort_edges orders heaviest first, then by source and target") {
    CHECK(sorted(edge_order::weight_source_target) ==
          std::vector<edge>{{1, 0, 5}, {0, 3, 4}, {1, 2, 3}, {0, 1, 2}, {0, 3, 2}, {1, 2, 1}});
}

TEST_CASE("sort_edges accepts no edges") {
    std::vector<edge> edges;
    sort_edges(edges, edge_order::source_target_weight);
    CHECK(edges.empty());
}
