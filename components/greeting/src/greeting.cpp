///
/// \file       greeting.cpp
/// \brief      Implementation of the graph_matching C++ library.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/greeting.hpp>

#include <string>
#include <string_view>

namespace graph_matching {

std::string make_greeting(std::string_view name) {
    auto greeting = std::string{"Hello, "};
    greeting.append(name);
    greeting.append("!");

    return greeting;
}

} // namespace graph_matching
