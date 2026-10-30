///
/// \file       matrix_market.cpp
/// \brief      Matrix Market coordinate file reader, private to the graph component.
///
/// \details    Reads the whole file into memory and parses it with
///             std::from_chars, which does no locale handling or
///             allocation per number. The format is described at
///             https://math.nist.gov/MatrixMarket/formats.html.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include "matrix_market.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace graph_matching::detail {

namespace {

///
/// \brief      Split the next line off the front of some text.
///
/// \param      text        Text to read from; advanced past the line.
///
/// \return     The line, without its line ending, or std::nullopt at the end.
///
std::optional<std::string_view> next_line(std::string_view &text) {
    if (text.empty()) {
        return std::nullopt;
    }

    const std::size_t end = text.find('\n');
    std::string_view line = text.substr(0, end);
    text.remove_prefix(end == std::string_view::npos ? text.size() : end + 1);

    if (line.ends_with('\r')) {
        line.remove_suffix(1);
    }

    return line;
}

/// Characters that separate tokens on a line.
constexpr std::string_view whitespace = " \t\r\n\f\v";

///
/// \brief      Split the next whitespace-separated token off the front of a line.
///
/// \param      line        Line to read from; advanced past the token.
///
/// \return     The token, or an empty view if the line has none left.
///
std::string_view next_token(std::string_view &line) {
    line.remove_prefix(std::min(line.find_first_not_of(whitespace), line.size()));

    const std::size_t end = std::min(line.find_first_of(whitespace), line.size());
    const std::string_view token = line.substr(0, end);
    line.remove_prefix(end);

    return token;
}

///
/// \brief      Check whether a line has only whitespace.
///
/// \param      line        Line to check.
///
/// \return     True if the line is blank.
///
bool is_blank(std::string_view line) {
    return line.find_first_not_of(whitespace) == std::string_view::npos;
}

///
/// \brief      Compare two words, ignoring ASCII case.
///
/// \param      a           First word.
/// \param      b           Second word, in lower case.
///
/// \return     True if the words match.
///
bool equals_lower(std::string_view a, std::string_view b) {
    return std::ranges::equal(a, b, [](char x, char y) {
        return std::tolower(static_cast<unsigned char>(x)) == static_cast<unsigned char>(y);
    });
}

///
/// \brief      Parse a whole token as an integer.
///
/// \param      token       Token to parse.
///
/// \return     The integer, or std::nullopt if the token is not one.
///
std::optional<std::int64_t> parse_integer(std::string_view token) {
    std::int64_t value = 0;
    const auto [end, error] = std::from_chars(token.begin(), token.end(), value);

    if (error != std::errc{} || end != token.end()) {
        return std::nullopt;
    }
    return value;
}

///
/// \brief      Parse a whole token as a weight.
///
/// \param      token       Token to parse; a leading '+' is allowed.
///
/// \return     The weight, or std::nullopt if the token is not a number.
///
std::optional<double> parse_weight(std::string_view token) {
    if (token.starts_with('+')) {
        token.remove_prefix(1);
    }

    double value = 0;
    const auto [end, error] = std::from_chars(token.begin(), token.end(), value);

    if (error != std::errc{} || end != token.end()) {
        return std::nullopt;
    }
    return value;
}

///
/// \brief      Check the banner line, such as "%%MatrixMarket matrix coordinate real general".
///
/// \param      line        First line of the file.
///
/// \return     True for a sparse (coordinate) matrix that is not complex or Hermitian.
///
bool is_supported_banner(std::string_view line) {
    const std::string_view banner = next_token(line);
    const std::string_view object = next_token(line);
    const std::string_view format = next_token(line);
    const std::string_view field = next_token(line);
    const std::string_view symmetry = next_token(line);

    return equals_lower(banner, "%%matrixmarket") && equals_lower(object, "matrix") &&
           equals_lower(format, "coordinate") &&
           (equals_lower(field, "real") || equals_lower(field, "integer") ||
            equals_lower(field, "pattern")) &&
           (equals_lower(symmetry, "general") || equals_lower(symmetry, "symmetric") ||
            equals_lower(symmetry, "skew-symmetric")) &&
           next_token(line).empty();
}

///
/// \brief      Read a whole file into memory.
///
/// \param      path        File to read.
///
/// \return     The file's contents, or std::nullopt if it cannot be read.
///
std::optional<std::string> read_file(const std::filesystem::path &path) {
    std::error_code error;
    const auto size = std::filesystem::file_size(path, error);
    if (error) {
        return std::nullopt;
    }

    std::ifstream file(path, std::ios::binary);
    std::string contents(size, '\0');

    if (!file.read(contents.data(), static_cast<std::streamsize>(size))) {
        return std::nullopt;
    }
    return contents;
}

} // namespace

std::expected<matrix_market_entries, graph::err>
read_matrix_market(const std::filesystem::path &path) {
    const std::optional<std::string> contents = read_file(path);
    if (!contents) {
        return std::unexpected(graph::err::file_error);
    }

    std::string_view text = *contents;

    const std::optional<std::string_view> banner = next_line(text);
    if (!banner || !is_supported_banner(*banner)) {
        return std::unexpected(graph::err::parse_error);
    }

    // Comments and blank lines may come before the size line.
    std::optional<std::string_view> size_line = next_line(text);
    while (size_line && (size_line->starts_with('%') || is_blank(*size_line))) {
        size_line = next_line(text);
    }
    if (!size_line) {
        return std::unexpected(graph::err::parse_error);
    }

    const auto rows = parse_integer(next_token(*size_line));
    const auto columns = parse_integer(next_token(*size_line));
    const auto entry_count = parse_integer(next_token(*size_line));
    constexpr std::int64_t max_vertices = std::numeric_limits<vertex_id>::max();

    if (!rows || !columns || !entry_count || !next_token(*size_line).empty() || *rows < 0 ||
        *columns < 0 || *entry_count < 0 || *rows > max_vertices || *columns > max_vertices) {
        return std::unexpected(graph::err::parse_error);
    }

    matrix_market_entries result;
    result.vertex_count = static_cast<vertex_id>(std::max(*rows, *columns));

    // Each entry line takes at least 4 bytes ("1 2\n"), so a size line
    // claiming more entries than that cannot reserve more than the file holds.
    const auto most_entries = static_cast<std::int64_t>(text.size() / 4);
    result.edges.reserve(static_cast<std::size_t>(std::min(*entry_count, most_entries)));

    for (std::int64_t i = 0; i < *entry_count; ++i) {
        std::optional<std::string_view> line = next_line(text);
        while (line && is_blank(*line)) {
            line = next_line(text);
        }
        if (!line) {
            return std::unexpected(graph::err::parse_error);
        }

        const auto row = parse_integer(next_token(*line));
        const auto column = parse_integer(next_token(*line));
        const std::string_view weight_token = next_token(*line);
        const auto weight = weight_token.empty() ? std::optional(1.0) : parse_weight(weight_token);

        if (!row || !column || !weight || !next_token(*line).empty() || *row < 1 ||
            *row > result.vertex_count || *column < 1 || *column > result.vertex_count) {
            return std::unexpected(graph::err::parse_error);
        }

        if (*row == *column) {
            continue;
        }

        const auto [smaller, larger] = std::minmax(*row, *column);
        result.edges.push_back({
            .source = static_cast<vertex_id>(smaller - 1),
            .target = static_cast<vertex_id>(larger - 1),
            .weight = *weight,
        });
    }

    return result;
}

} // namespace graph_matching::detail
