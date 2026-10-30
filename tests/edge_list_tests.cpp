///
/// \file       edge_list_tests.cpp
/// \brief      Unit tests for edge lists and their sort orders.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/edge_list.h>

#include <doctest/doctest.h>

#include <algorithm>
#include <array>
#include <span>
#include <vector>

namespace {

///
/// \brief      Edges with ties in every key, in no particular order.
///
constexpr std::array<edge, 6> unsorted_edges{{
    {.source = 1, .target = 2, .weight = 1.0},
    {.source = 0, .target = 3, .weight = 2.0},
    {.source = 1, .target = 0, .weight = 5.0},
    {.source = 0, .target = 1, .weight = 2.0},
    {.source = 1, .target = 2, .weight = 3.0},
    {.source = 0, .target = 3, .weight = 4.0},
}};

///
/// \brief      Sort a copy of unsorted_edges.
///
/// \param      order       Key order.
///
/// \return     The sorted edges.
///
std::vector<edge> sorted(edge_order order) {
    edge_list list;
    REQUIRE(edge_list_init(&list, static_cast<int>(unsorted_edges.size())) == STATUS_OK);

    const std::span edges(list.edges, unsorted_edges.size());
    std::ranges::copy(unsorted_edges, edges.begin());
    edge_list_sort(&list, order);

    std::vector<edge> result(edges.begin(), edges.end());
    edge_list_destroy(&list);
    return result;
}

///
/// \brief      Format edges as (source, target, weight) triples for comparison.
///
/// \param      edges       Edges to format.
///
/// \return     One triple per edge.
///
std::vector<std::array<double, 3>> triples(std::span<const edge> edges) {
    std::vector<std::array<double, 3>> result;
    for (const edge &e : edges) {
        result.push_back({static_cast<double>(e.source), static_cast<double>(e.target), e.weight});
    }
    return result;
}

} // namespace

TEST_CASE("edge_list_sort orders by source, target, then heaviest first") {
    CHECK(triples(sorted(EDGE_ORDER_SOURCE_TARGET_WEIGHT)) ==
          std::vector<std::array<double, 3>>{
              {0, 1, 2}, {0, 3, 4}, {0, 3, 2}, {1, 0, 5}, {1, 2, 3}, {1, 2, 1}});
}

TEST_CASE("edge_list_sort orders by source, heaviest first, then target") {
    CHECK(triples(sorted(EDGE_ORDER_SOURCE_WEIGHT_TARGET)) ==
          std::vector<std::array<double, 3>>{
              {0, 3, 4}, {0, 1, 2}, {0, 3, 2}, {1, 0, 5}, {1, 2, 3}, {1, 2, 1}});
}

TEST_CASE("edge_list_sort orders heaviest first, then by source and target") {
    CHECK(triples(sorted(EDGE_ORDER_WEIGHT_SOURCE_TARGET)) ==
          std::vector<std::array<double, 3>>{
              {1, 0, 5}, {0, 3, 4}, {1, 2, 3}, {0, 1, 2}, {0, 3, 2}, {1, 2, 1}});
}

TEST_CASE("edge_list_init accepts zero edges and rejects negative lengths") {
    edge_list list;

    REQUIRE(edge_list_init(&list, 0) == STATUS_OK);
    CHECK(list.length == 0);
    edge_list_sort(&list, EDGE_ORDER_SOURCE_TARGET_WEIGHT);
    edge_list_destroy(&list);
    CHECK(list.edges == nullptr);

    CHECK(edge_list_init(&list, -1) == STATUS_INVALID_ARGUMENT);
    CHECK(list.edges == nullptr);
}
