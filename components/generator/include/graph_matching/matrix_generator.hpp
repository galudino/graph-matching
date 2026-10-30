///
/// \file       matrix_generator.hpp
/// \brief      Random test matrices in Matrix Market format.
///
/// \details    Generates random undirected weighted graphs and writes them as
///             symmetric Matrix Market files that read_mm() accepts.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_MATRIX_GENERATOR_HPP
#define GRAPH_MATCHING_MATRIX_GENERATOR_HPP

#include <cstdint>
#include <iosfwd>
#include <span>
#include <vector>

namespace graph_matching {

///
/// \brief      One undirected edge, stored as a lower-triangle matrix entry.
///
struct edge {
    int row;    ///< Larger endpoint (0-based).
    int col;    ///< Smaller endpoint (0-based).
    int weight; ///< Edge weight, at least 1.
};

///
/// \brief      Parameters for generate_edges().
///
struct generator_options {
    int vertices = 1000;    ///< Number of vertices (at least 2).
    int average_degree = 8; ///< Average number of neighbors per vertex (at least 1).
    int max_weight = 10;    ///< Weights are drawn uniformly from 1 to max_weight.
    std::uint64_t seed = 1; ///< Random seed; the same options produce the same edges.
};

///
/// \brief      Generate a random graph's edges.
///
/// \details    Draws vertices * average_degree / 2 edges between uniformly
///             random pairs of distinct vertices. A pair can be drawn twice;
///             read_mm() keeps the heavier copy, so the graph can have
///             slightly fewer edges than entries.
///
/// \param      options     Size, weights, and seed.
///
/// \return     The edges, in generation order.
///
/// \throws     std::invalid_argument if an option is out of range, or the
///             matrix would have more entries than read_mm() can hold.
///
[[nodiscard]] std::vector<edge> generate_edges(const generator_options &options);

///
/// \brief      Write edges as a symmetric Matrix Market coordinate file.
///
/// \param      out         Stream to write to.
/// \param      options     Options the edges were generated with; recorded in
///                         a comment line, and options.vertices sets the
///                         matrix size.
/// \param      edges       Edges to write.
///
void write_matrix_market(std::ostream &out, const generator_options &options,
                         std::span<const edge> edges);

} // namespace graph_matching

#endif // GRAPH_MATCHING_MATRIX_GENERATOR_HPP
