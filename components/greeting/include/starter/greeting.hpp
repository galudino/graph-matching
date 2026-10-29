///
/// \file       greeting.hpp
/// \brief      Public interface for the starter C++ library.
///
/// \details    Demonstrates a separately compiled C++ library and
///             a constexpr function defined in a header.
///
/// \author     galudino
/// \date       2026-10-23
///

#ifndef STARTER_GREETING_HPP
#define STARTER_GREETING_HPP

#include <string>
#include <string_view>

namespace starter {

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

} // namespace starter

#endif // STARTER_GREETING_HPP
