///
/// \file       support_tests.cpp
/// \brief      Unit tests for status codes, prefix sums, and timing.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/prefix_sum.h>
#include <graph_matching/status_code.h>
#include <graph_matching/stopwatch.h>

#include <doctest/doctest.h>

#include <array>
#include <set>
#include <string>

TEST_CASE("prefix_sum_exclusive starts at zero and ends with the total") {
    const std::array values{3, 0, 2, 5};
    std::array<int, 5> sums{};

    prefix_sum_exclusive(values.data(), sums.data(), static_cast<int>(values.size()));

    CHECK(sums == std::array{0, 3, 3, 5, 10});
}

TEST_CASE("prefix_sum_exclusive of nothing is a single zero") {
    std::array sums{-1};

    prefix_sum_exclusive(nullptr, sums.data(), 0);

    CHECK(sums[0] == 0);
}

TEST_CASE("timespec_difference_ms converts across a second boundary") {
    const timespec start{.tv_sec = 1, .tv_nsec = 900'000'000};
    const timespec end{.tv_sec = 3, .tv_nsec = 150'000'000};

    CHECK(timespec_difference_ms(&start, &end) == doctest::Approx(1250.0));
}

TEST_CASE("stopwatch measures a non-negative elapsed time") {
    stopwatch timer;
    stopwatch_start(&timer);

    CHECK(stopwatch_elapsed_ms(&timer) >= 0.0);
}

TEST_CASE("status_code_string describes every code differently") {
    const std::array codes{STATUS_OK,         STATUS_INVALID_ARGUMENT, STATUS_OUT_OF_MEMORY,
                           STATUS_FILE_ERROR, STATUS_PARSE_ERROR,      STATUS_INVALID_MATCHING};

    std::set<std::string> descriptions;
    for (const status_code code : codes) {
        descriptions.emplace(status_code_string(code));
    }

    CHECK(descriptions.size() == codes.size());
}
