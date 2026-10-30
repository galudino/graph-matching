///
/// \file       matrix_market.hpp
/// \brief      Matrix Market coordinate file reader, private to the graph component.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_CPP_MATRIX_MARKET_HPP
#define GRAPH_MATCHING_CPP_MATRIX_MARKET_HPP

#include <graph_matching_cpp/edge.hpp>
#include <graph_matching_cpp/graph.hpp>

#include <expected>
#include <filesystem>
#include <vector>

namespace graph_matching::detail {

///
/// \brief      The entries of a Matrix Market file, as normalized edges.
///
struct matrix_market_entries {
    vertex_id vertex_count = 0; ///< The larger of the row and column counts.
    std::vector<edge> edges;    ///< Source < target, 0-based; self-loops dropped.
};

///
/// \brief      Read the entries of a Matrix Market coordinate file.
///
/// \details    Accepts real, integer, and pattern matrices, in any symmetry
///             but complex or Hermitian. An entry without a value gets
///             weight 1.
///
/// \param      path        Path to the .mtx file.
///
/// \return     The entries, graph::err::file_error, or graph::err::parse_error.
///
[[nodiscard]] std::expected<matrix_market_entries, graph::err>
read_matrix_market(const std::filesystem::path &path);

} // namespace graph_matching::detail

#endif // GRAPH_MATCHING_CPP_MATRIX_MARKET_HPP
