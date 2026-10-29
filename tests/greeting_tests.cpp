///
/// \file       greeting_tests.cpp
/// \brief      Unit tests for the greeting library.
///
/// \author     galudino
/// \date       2026-10-24
///

#include <starter/greeting.hpp>

#include <doctest/doctest.h>

TEST_CASE("make_greeting formats the recipient's name") {
    CHECK(starter::make_greeting("World") == "Hello, World!");
    CHECK(starter::make_greeting("") == "Hello, !");
}

TEST_CASE("is_valid_initial accepts only ASCII letters") {
    static_assert(starter::is_valid_initial('A'));
    static_assert(starter::is_valid_initial('z'));
    static_assert(!starter::is_valid_initial('7'));

    CHECK(starter::is_valid_initial('M'));
    CHECK(starter::is_valid_initial('m'));
    CHECK_FALSE(starter::is_valid_initial('5'));
    CHECK_FALSE(starter::is_valid_initial(' '));
}
