///
/// \file       graph.cpp
/// \brief      Undirected weighted graph stored as adjacency arrays.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching_cpp/graph.hpp>

#include "matrix_market.hpp"

#include <graph_matching_cpp/edge.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <expected>
#include <filesystem>
#include <numeric>
#include <print>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace graph_matching {

namespace {

///
/// \brief      Store each distinct edge in both directions.
///
/// \details    Takes the edges by value, so they are released on return.
///
/// \param      undirected      Normalized edges: source < target.
///
/// \return     Two directed edges per distinct pair, keeping the heaviest copy.
///
std::vector<edge> directed_edges(std::vector<edge> undirected) {
    // Sorting puts copies of the same pair together, heaviest first.
    sort_edges(undirected, edge_order::source_target_weight);

    std::vector<edge> directed;
    directed.reserve(2 * undirected.size());

    for (std::size_t i = 0; i < undirected.size(); ++i) {
        const edge &e = undirected[i];
        if (i > 0 && e.source == undirected[i - 1].source && e.target == undirected[i - 1].target) {
            continue;
        }

        directed.push_back(e);
        directed.push_back({.source = e.target, .target = e.source, .weight = e.weight});
    }

    return directed;
}

} // namespace

std::expected<graph, graph::err> graph::from_edges(vertex_id vertex_count,
                                                   std::span<const edge> edges) {
    if (vertex_count < 0) {
        return std::unexpected(err::invalid_argument);
    }

    std::vector<edge> undirected;
    undirected.reserve(edges.size());

    for (const edge &e : edges) {
        if (e.source < 0 || e.source >= vertex_count || e.target < 0 || e.target >= vertex_count) {
            return std::unexpected(err::invalid_argument);
        }

        if (e.source == e.target) {
            continue;
        }

        const auto [smaller, larger] = std::minmax(e.source, e.target);
        undirected.push_back({.source = smaller, .target = larger, .weight = e.weight});
    }

    return build(vertex_count, std::move(undirected));
}

std::expected<graph, graph::err> graph::from_matrix_market(const std::filesystem::path &path) {
    // The reader already drops self-loops and puts the smaller endpoint first.
    return detail::read_matrix_market(path).transform([](detail::matrix_market_entries entries) {
        return build(entries.vertex_count, std::move(entries.edges));
    });
}

graph graph::build(vertex_id vertex_count, std::vector<edge> undirected_edges) {
    std::vector<edge> directed = directed_edges(std::move(undirected_edges));

    // Group by vertex, strongest neighbor first.
    sort_edges(directed, edge_order::source_weight_target);

    graph result;
    result.m_vertex_count = vertex_count;
    result.m_neighbor_offsets.assign(static_cast<std::size_t>(vertex_count) + 1, 0);
    result.m_neighbors.reserve(directed.size());
    result.m_weights.reserve(directed.size());

    // Count each vertex's degree one slot ahead, so a running sum turns the
    // counts into the start of each list.
    for (const edge &e : directed) {
        ++result.m_neighbor_offsets[static_cast<std::size_t>(e.source) + 1];
        result.m_neighbors.push_back(e.target);
        result.m_weights.push_back(e.weight);
    }

    std::inclusive_scan(result.m_neighbor_offsets.begin(), result.m_neighbor_offsets.end(),
                        result.m_neighbor_offsets.begin());

    return result;
}

void graph::print(std::FILE *stream) const {
    std::println(stream, "{} vertices, {} edges", m_vertex_count, edge_count());

    for (vertex_id v = 0; v < m_vertex_count; ++v) {
        std::print(stream, "vertex {}, degree {}:", v, degree(v));

        const auto vertex_neighbors = neighbors(v);
        const auto weights = neighbor_weights(v);
        for (std::size_t i = 0; i < vertex_neighbors.size(); ++i) {
            std::print(stream, " ({}, {:.4f})", vertex_neighbors[i], weights[i]);
        }

        std::print(stream, "\n");
    }
}

std::string_view to_string(graph::err error) noexcept {
    switch (error) {
    case graph::err::invalid_argument:
        return "invalid argument";
    case graph::err::file_error:
        return "file could not be opened or read";
    case graph::err::parse_error:
        return "malformed Matrix Market file";
    }
    return "unknown error";
}

} // namespace graph_matching
