///
/// \file       graph.hpp
/// \brief      Undirected weighted graph stored as adjacency arrays.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#ifndef GRAPH_MATCHING_CPP_GRAPH_HPP
#define GRAPH_MATCHING_CPP_GRAPH_HPP

#include <graph_matching_cpp/edge.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <expected>
#include <filesystem>
#include <span>
#include <string_view>
#include <vector>

namespace graph_matching {

///
/// \brief      Undirected weighted graph stored as adjacency arrays.
///
/// \details    The neighbors of vertex v are listed strongest first: by
///             descending weight, then ascending vertex ID. Every edge
///             appears in both of its endpoints' lists. Self-loops are
///             dropped, and when the same pair appears more than once, the
///             heaviest copy is kept.
///
///             A graph is immutable once built. Build one with
///             from_edges() or from_matrix_market(); both report failure
///             through std::expected rather than exceptions, apart from
///             std::bad_alloc.
///
class graph {
public:
    ///
    /// \brief      Why a graph could not be built.
    ///
    enum class err : std::uint8_t {
        invalid_argument, ///< A vertex count or endpoint was out of range.
        file_error,       ///< The file could not be opened or read.
        parse_error,      ///< The file's contents were malformed.
    };

    ///
    /// \brief      Build a graph from a list of edges.
    ///
    /// \details    Each edge connects its source and target in both
    ///             directions.
    ///
    /// \param      vertex_count    Number of vertices; at least 0.
    /// \param      edges           Edges, with endpoints from 0 to vertex_count - 1.
    ///
    /// \return     The graph, or err::invalid_argument.
    ///
    [[nodiscard]] static std::expected<graph, err> from_edges(vertex_id vertex_count,
                                                              std::span<const edge> edges);

    ///
    /// \brief      Read a graph from a Matrix Market coordinate file.
    ///
    /// \details    Entry (i, j) becomes an edge between vertices i - 1 and
    ///             j - 1, with the entry's value as its weight, or 1 for a
    ///             pattern matrix.
    ///
    /// \param      path        Path to the .mtx file.
    ///
    /// \return     The graph, err::file_error, or err::parse_error.
    ///
    [[nodiscard]] static std::expected<graph, err>
    from_matrix_market(const std::filesystem::path &path);

    ///
    /// \return     Number of vertices, numbered 0 to vertex_count() - 1.
    ///
    [[nodiscard]] vertex_id vertex_count() const noexcept { return m_vertex_count; }

    ///
    /// \return     Number of undirected edges.
    ///
    [[nodiscard]] std::size_t edge_count() const noexcept { return m_neighbors.size() / 2; }

    ///
    /// \brief      Number of neighbors of a vertex.
    ///
    /// \param      vertex      Vertex ID, from 0 to vertex_count() - 1.
    ///
    /// \return     The vertex's degree.
    ///
    [[nodiscard]] std::size_t degree(vertex_id vertex) const noexcept {
        return offset(vertex + 1) - offset(vertex);
    }

    ///
    /// \brief      Neighbors of a vertex, strongest first.
    ///
    /// \param      vertex      Vertex ID, from 0 to vertex_count() - 1.
    ///
    /// \return     degree(vertex) neighbor IDs.
    ///
    [[nodiscard]] std::span<const vertex_id> neighbors(vertex_id vertex) const noexcept {
        return std::span(m_neighbors).subspan(offset(vertex), degree(vertex));
    }

    ///
    /// \brief      Weights of the edges to a vertex's neighbors.
    ///
    /// \param      vertex      Vertex ID, from 0 to vertex_count() - 1.
    ///
    /// \return     degree(vertex) weights, in the same order as neighbors().
    ///
    [[nodiscard]] std::span<const double> neighbor_weights(vertex_id vertex) const noexcept {
        return std::span(m_weights).subspan(offset(vertex), degree(vertex));
    }

    ///
    /// \brief      Write every vertex's weighted neighbor list.
    ///
    /// \param      stream      Stream to write to, such as stderr.
    ///
    void print(std::FILE *stream) const;

private:
    graph() = default;

    ///
    /// \brief      Build a graph from normalized edges.
    ///
    /// \details    Every edge must have source < target, both in range.
    ///             Takes the edges by value, so their memory is released
    ///             once the adjacency arrays are built.
    ///
    /// \param      vertex_count        Number of vertices.
    /// \param      undirected_edges    Normalized edges.
    ///
    /// \return     The graph.
    ///
    [[nodiscard]] static graph build(vertex_id vertex_count, std::vector<edge> undirected_edges);

    ///
    /// \param      vertex      Vertex ID, from 0 to vertex_count(); vertex_count()
    ///                         gives the end of the last list.
    ///
    /// \return     Where the vertex's neighbor list starts.
    ///
    [[nodiscard]] std::size_t offset(vertex_id vertex) const noexcept {
        return m_neighbor_offsets[static_cast<std::size_t>(vertex)];
    }

    vertex_id m_vertex_count = 0;                ///< Number of vertices.
    std::vector<std::size_t> m_neighbor_offsets; ///< Start of each list; vertex_count + 1 entries.
    std::vector<vertex_id> m_neighbors;          ///< All neighbor lists, concatenated.
    std::vector<double> m_weights;               ///< Weight of each entry in m_neighbors.
};

///
/// \brief      Describe a graph error.
///
/// \param      error       Error to describe.
///
/// \return     A short, static, human-readable description.
///
[[nodiscard]] std::string_view to_string(graph::err error) noexcept;

} // namespace graph_matching

#endif // GRAPH_MATCHING_CPP_GRAPH_HPP
