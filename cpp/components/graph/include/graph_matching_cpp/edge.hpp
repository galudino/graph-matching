///
/// \file       edge.hpp
/// \brief      Weighted edges, and sorting them by different keys.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_CPP_EDGE_HPP
#define GRAPH_MATCHING_CPP_EDGE_HPP

#include <cstdint>
#include <span>

namespace graph_matching {

///
/// \brief      Vertex ID, from 0 to vertex_count - 1.
///
/// \details    32 bits, like the C implementation's int: neighbor lists
///             hold one per edge endpoint, so a narrower type halves their
///             size compared with std::size_t.
///
using vertex_id = std::int32_t;

///
/// \brief      One weighted edge between two vertices.
///
struct edge {
    vertex_id source; ///< First endpoint.
    vertex_id target; ///< Second endpoint.
    double weight;    ///< Edge weight.

    ///
    /// \brief      Compare every field; used by tests.
    ///
    [[nodiscard]] friend constexpr bool operator==(const edge &, const edge &) = default;
};

///
/// \brief      Key orders for sort_edges().
///
/// \details    Each name lists the keys from most to least significant.
///             Vertex IDs sort ascending; weights sort descending, so the
///             heaviest edge comes first.
///
enum class edge_order : std::uint8_t {
    source_target_weight, ///< By source, then target, then heaviest first.
    source_weight_target, ///< By source, then heaviest first, then target.
    weight_source_target, ///< Heaviest first, then by source, then target.
};

///
/// \brief      Sort edges in place.
///
/// \param      edges       Edges to sort.
/// \param      order       Key order.
///
void sort_edges(std::span<edge> edges, edge_order order);

} // namespace graph_matching

#endif // GRAPH_MATCHING_CPP_EDGE_HPP
