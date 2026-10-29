///
/// \file       greeting.hpp
/// \brief      Public interface for the graph_matching C++ library.
///
/// \details    Demonstrates a separately compiled C++ library and
///             a constexpr function defined in a header.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#ifndef GRAPH_MATCHING_GREETING_HPP
#define GRAPH_MATCHING_GREETING_HPP

#include <string>
#include <string_view>

namespace graph_matching {

///
/// \brief      Generates a greeting for the specified name.
///
/// \param      name        Name of the recipient.
///
/// \return     A formatted greeting string.
///
[[nodiscard]] std::string make_greeting(std::string_view name);

///
/// \brief      Determine whether a character can begin an ASCII name.
///
/// \param      character  Character to examine.
///
/// \return     True if the character is an ASCII letter.
///
[[nodiscard]] constexpr bool is_valid_initial(char character) noexcept {
    return (character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z');
}

} // namespace graph_matching

#endif // GRAPH_MATCHING_GREETING_HPP
