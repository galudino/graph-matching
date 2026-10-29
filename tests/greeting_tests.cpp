///
/// \file       greeting_tests.cpp
/// \brief      Unit tests for the greeting library.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/greeting.hpp>

#include <doctest/doctest.h>

TEST_CASE("make_greeting formats the recipient's name") {
    CHECK(graph_matching::make_greeting("World") == "Hello, World!");
    CHECK(graph_matching::make_greeting("") == "Hello, !");
}

TEST_CASE("is_valid_initial accepts only ASCII letters") {
    static_assert(graph_matching::is_valid_initial('A'));
    static_assert(graph_matching::is_valid_initial('z'));
    static_assert(!graph_matching::is_valid_initial('7'));

    CHECK(graph_matching::is_valid_initial('M'));
    CHECK(graph_matching::is_valid_initial('m'));
    CHECK_FALSE(graph_matching::is_valid_initial('5'));
    CHECK_FALSE(graph_matching::is_valid_initial(' '));
}
