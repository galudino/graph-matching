///
/// \file       utils_tests.cpp
/// \brief      Unit tests for the harness utilities.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/match.h>
#include <graph_matching/utils.h>

#include <doctest/doctest.h>

#include <array>
#include <cstdio>
#include <filesystem>
#include <string>

TEST_CASE("exclusive_prefix_sum starts at zero and ends with the total") {
    const std::array num{3, 0, 2, 5};
    std::array<int, 5> sum{};

    exclusive_prefix_sum(num.data(), sum.data(), static_cast<int>(num.size()));

    CHECK(sum == std::array{0, 3, 3, 5, 10});
}

TEST_CASE("inclusive_prefix_sum includes each element") {
    const std::array num{3, 0, 2, 5};
    std::array<int, 4> sum{};

    inclusive_prefix_sum(num.data(), sum.data(), static_cast<int>(num.size()));

    CHECK(sum == std::array{3, 3, 5, 10});
}

TEST_CASE("swap_int_array exchanges the pointers") {
    std::array a{1};
    std::array b{2};
    int *first = a.data();
    int *second = b.data();

    swap_int_array(&first, &second);

    CHECK(first == b.data());
    CHECK(second == a.data());
}

TEST_CASE("get_time converts to milliseconds across a second boundary") {
    const timespec start{.tv_sec = 1, .tv_nsec = 900'000'000};
    const timespec end{.tv_sec = 3, .tv_nsec = 150'000'000};

    CHECK(get_time(&start, &end) == doctest::Approx(1250.0));
}

TEST_CASE("write_match_result and read_res round-trip a result") {
    const auto path = std::filesystem::temp_directory_path() / "graph_matching_result_test.out";
    const std::array<int, 5> written{1, 0, NO_MATCHED_NODES, 4, 3};

    write_match_result(path.c_str(), written.data(), static_cast<int>(written.size()));

    std::array<int, 5> read{};
    int *read_ptr = read.data();
    read_res(path.c_str(), static_cast<int>(read.size()), &read_ptr);

    CHECK(read == written);

    std::filesystem::remove(path);
}
