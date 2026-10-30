///
/// \file       matching_tests.cpp
/// \brief      Unit tests for matchings: greedy, validation, and file I/O.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/graph.h>
#include <graph_matching/matching.h>

#include "test_files.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <span>
#include <vector>

namespace {

///
/// \brief      data/test.mtx and a matching of it, released automatically.
///
struct test_matrix_fixture {
    graph input{};     ///< The 9-vertex example graph.
    matching result{}; ///< A matching of input, all VERTEX_UNMATCHED to start.

    test_matrix_fixture() {
        REQUIRE(graph_init_from_matrix_market(&input, GRAPH_MATCHING_DATA_DIR "/test.mtx") ==
                STATUS_OK);
        REQUIRE(matching_init(&result, input.vertex_count) == STATUS_OK);
    }

    test_matrix_fixture(const test_matrix_fixture &) = delete;
    test_matrix_fixture &operator=(const test_matrix_fixture &) = delete;
    test_matrix_fixture(test_matrix_fixture &&) = delete;
    test_matrix_fixture &operator=(test_matrix_fixture &&) = delete;

    ~test_matrix_fixture() {
        matching_deinit(&result);
        graph_deinit(&input);
    }

    ///
    /// \return     A copy of every partner value.
    ///
    [[nodiscard]] std::vector<int> partners() const {
        const std::span values(result.partners, static_cast<std::size_t>(result.vertex_count));
        return {values.begin(), values.end()};
    }
};

///
/// \brief      Replace every partner value of a matching.
///
/// \param      m           Matching to change.
/// \param      values      One value per vertex.
///
void set_partners(matching &m, const std::vector<int> &values) {
    REQUIRE(values.size() == static_cast<std::size_t>(m.vertex_count));
    std::ranges::copy(values, m.partners);
}

} // namespace

TEST_CASE_FIXTURE(test_matrix_fixture, "matching_init starts every vertex unmatched") {
    CHECK(partners() == std::vector<int>(9, VERTEX_UNMATCHED));
    CHECK(matching_pair_count(&result) == 0);
}

TEST_CASE_FIXTURE(test_matrix_fixture, "matching_run_greedy takes heaviest edges first") {
    REQUIRE(matching_run_greedy(&result, &input) == STATUS_OK);

    // Edges of weight 4 first: (1, 4) and (2, 5). Then 3: (3, 4) and (4, 7)
    // are blocked by 4. Then 2: (0, 3) and (7, 8). Vertex 6 has no free neighbor.
    CHECK(partners() == std::vector<int>{3, 4, 5, 0, 1, 2, VERTEX_UNMATCHABLE, 8, 7});
    CHECK(matching_validate(&result, &input) == STATUS_OK);
    CHECK(matching_is_maximal(&result, &input));
    CHECK(matching_pair_count(&result) == 4);
    CHECK(matching_weight(&result, &input) == doctest::Approx(12.0));
}

TEST_CASE_FIXTURE(test_matrix_fixture, "matching_validate rejects broken matchings") {
    const int x = VERTEX_UNMATCHABLE;

    SUBCASE("a vertex still unmatched") {
        set_partners(result, {3, 4, 5, 0, 1, 2, VERTEX_UNMATCHED, 8, 7});
    }
    SUBCASE("a match that is not mutual") { set_partners(result, {3, 4, 5, 6, 1, 2, x, 8, 7}); }
    SUBCASE("a matched pair that shares no edge") {
        set_partners(result, {8, 4, 5, x, 1, 2, x, x, 0});
    }
    SUBCASE("a partner out of range") { set_partners(result, {9, 4, 5, x, 1, 2, x, 8, 7}); }

    CHECK(matching_validate(&result, &input) == STATUS_INVALID_MATCHING);
}

TEST_CASE_FIXTURE(test_matrix_fixture,
                  "matching_is_maximal finds an edge left between free vertices") {
    const int x = VERTEX_UNMATCHABLE;
    set_partners(result, {x, 4, 5, x, 1, 2, x, 8, 7});

    CHECK(matching_validate(&result, &input) == STATUS_OK);
    CHECK_FALSE(matching_is_maximal(&result, &input));
}

TEST_CASE_FIXTURE(test_matrix_fixture,
                  "matching_write and matching_init_from_file round-trip a matching") {
    REQUIRE(matching_run_greedy(&result, &input) == STATUS_OK);
    const graph_matching::testing::temporary_file file("graph_matching_result.out");

    REQUIRE(matching_write(&result, file.c_str()) == STATUS_OK);

    matching read_back;
    REQUIRE(matching_init_from_file(&read_back, file.c_str(), 9) == STATUS_OK);
    const std::span values(read_back.partners, 9);
    CHECK(std::vector(values.begin(), values.end()) == partners());
    matching_deinit(&read_back);

    CHECK(matching_init_from_file(&read_back, file.c_str(), 10) == STATUS_PARSE_ERROR);
    CHECK(read_back.partners == nullptr);
}

TEST_CASE("matching_run_greedy rejects a matching of the wrong size") {
    graph input;
    REQUIRE(graph_init_from_matrix_market(&input, GRAPH_MATCHING_DATA_DIR "/test.mtx") ==
            STATUS_OK);
    matching result;
    REQUIRE(matching_init(&result, 3) == STATUS_OK);

    CHECK(matching_run_greedy(&result, &input) == STATUS_INVALID_ARGUMENT);
    CHECK(matching_validate(&result, &input) == STATUS_INVALID_MATCHING);

    matching_deinit(&result);
    graph_deinit(&input);
}
