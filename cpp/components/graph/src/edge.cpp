///
/// \file       edge.cpp
/// \brief      Weighted edges, and sorting them by different keys.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching_cpp/edge.hpp>

#include <algorithm>
#include <span>

namespace graph_matching {

void sort_edges(std::span<edge> edges, edge_order order) {
    // One lambda per order, so each sort inlines its comparison. (The C
    // version moved off qsort() for the same reason: a comparison called
    // through a pointer made reading large graphs twice as slow.)
    switch (order) {
    case edge_order::source_target_weight:
        std::ranges::sort(edges, [](const edge &a, const edge &b) {
            if (a.source != b.source) {
                return a.source < b.source;
            }
            if (a.target != b.target) {
                return a.target < b.target;
            }
            return a.weight > b.weight;
        });
        break;

    case edge_order::source_weight_target:
        std::ranges::sort(edges, [](const edge &a, const edge &b) {
            if (a.source != b.source) {
                return a.source < b.source;
            }
            if (a.weight != b.weight) {
                return a.weight > b.weight;
            }
            return a.target < b.target;
        });
        break;

    case edge_order::weight_source_target:
        std::ranges::sort(edges, [](const edge &a, const edge &b) {
            if (a.weight != b.weight) {
                return a.weight > b.weight;
            }
            if (a.source != b.source) {
                return a.source < b.source;
            }
            return a.target < b.target;
        });
        break;
    }
}

} // namespace graph_matching
