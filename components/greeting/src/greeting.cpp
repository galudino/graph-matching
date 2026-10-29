///
/// \file       greeting.cpp
/// \brief      Implementation of the starter C++ library.
///
/// \author     galudino
/// \date       2026-10-23
///

#include <starter/greeting.hpp>

#include <string>
#include <string_view>

namespace starter {

std::string make_greeting(std::string_view name) {
    auto greeting = std::string{"Hello, "};
    greeting.append(name);
    greeting.append("!");

    return greeting;
}

} // namespace starter
