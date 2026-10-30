///
/// \file       handshake_tests.cpp
/// \brief      Tests for the assignment: worklist, one-way, and N-way functions.
///
/// \details    Built only with -DGRAPH_MATCHING_TEST_ASSIGNMENT=ON, since they
///             fail until the assignment is implemented. The step tests call
///             one thread's functions at a time, in thread order, which is
///             valid because every step only reads what earlier steps wrote.
///             The examples match docs/assignment.md.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-30
///

#include <graph_matching/graph.h>
#include <graph_matching/handshake.h>
#include <graph_matching/handshake_worker.h>
#include <graph_matching/matching.h>
#include <graph_matching/matrix_generator.hpp>
#include <graph_matching/n_way.h>
#include <graph_matching/one_way.h>
#include <graph_matching/worklist.h>

#include "../test_files.hpp"

#include <doctest/doctest.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <numeric>
#include <span>
#include <vector>

namespace {

///
/// \brief      Shared state and workers for calling the steps directly, without threads.
///
class step_harness {
public:
    ///
    /// \brief      Set up every vertex of a graph as active.
    ///
    /// \param      input           Graph to match.
    /// \param      thread_count    Number of simulated threads.
    /// \param      hand_count      Hands per vertex.
    ///
    step_harness(const graph &input, int thread_count, int hand_count)
        : m_partners(vertices(input), VERTEX_UNMATCHED), m_strongest(vertices(input), -99),
          m_candidates(vertices(input) * static_cast<std::size_t>(hand_count), -99),
          m_candidate_counts(vertices(input), -99),
          m_unmatched_counts(static_cast<std::size_t>(thread_count), 0),
          m_write_offsets(static_cast<std::size_t>(thread_count) + 1, 0), m_active(vertices(input)),
          m_next_active(vertices(input), -99) {
        std::iota(m_active.begin(), m_active.end(), 0);

        m_shared.input_graph = &input;
        m_shared.partners = m_partners.data();
        m_shared.thread_count = thread_count;
        m_shared.hand_count = hand_count;
        m_shared.strongest_neighbor = m_strongest.data();
        m_shared.candidates = m_candidates.data();
        m_shared.candidate_counts = m_candidate_counts.data();
        m_shared.unmatched_counts = m_unmatched_counts.data();
        m_shared.write_offsets = m_write_offsets.data();

        for (int i = 0; i < thread_count; ++i) {
            m_workers.push_back({.shared = &m_shared,
                                 .thread_index = i,
                                 .active_count = input.vertex_count,
                                 .active_vertices = m_active.data(),
                                 .next_active_vertices = m_next_active.data(),
                                 .pass_index = 0});
        }
    }

    ///
    /// \brief      Run one step on every simulated thread, in order.
    ///
    /// \param      step        Assignment function to run.
    ///
    void run(const std::function<void(const handshake_worker *)> &step) const {
        for (const handshake_worker &worker : m_workers) {
            step(&worker);
        }
    }

    std::vector<int> m_partners;             ///< Result.
    std::vector<int> m_strongest;            ///< strongest_neighbor.
    std::vector<int> m_candidates;           ///< N-way candidates.
    std::vector<int> m_candidate_counts;     ///< N-way candidate counts.
    std::vector<int> m_unmatched_counts;     ///< Per-thread unmatched counts.
    std::vector<int> m_write_offsets;        ///< Per-thread write offsets.
    std::vector<int> m_active;               ///< Current worklist.
    std::vector<int> m_next_active;          ///< Next worklist.
    handshake_shared m_shared{};             ///< Shared state.
    std::vector<handshake_worker> m_workers; ///< One per simulated thread.

private:
    static std::size_t vertices(const graph &input) {
        return static_cast<std::size_t>(input.vertex_count);
    }
};

///
/// \brief      Read data/test.mtx.
///
/// \return     The graph; release it with graph_deinit().
///
graph read_test_matrix() {
    graph input{};
    REQUIRE(graph_init_from_matrix_market(&input, GRAPH_MATCHING_DATA_DIR "/test.mtx") ==
            STATUS_OK);
    return input;
}

///
/// \brief      Run handshake_match() and return the partners.
///
/// \param      input           Graph to match.
/// \param      thread_count    Number of threads.
/// \param      hand_count      Hands per vertex.
///
/// \return     The partner of every vertex.
///
std::vector<int> run_handshake(const graph &input, int thread_count, int hand_count) {
    matching result{};
    REQUIRE(matching_init(&result, input.vertex_count) == STATUS_OK);

    const handshake_options options{.thread_count = thread_count, .hand_count = hand_count};
    REQUIRE(handshake_match(&result, &input, &options, nullptr) == STATUS_OK);
    CHECK(matching_validate(&result, &input) == STATUS_OK);
    CHECK(matching_is_maximal(&result, &input));

    const std::span partners(result.partners, static_cast<std::size_t>(result.vertex_count));
    std::vector<int> copy(partners.begin(), partners.end());
    matching_deinit(&result);
    return copy;
}

} // namespace

TEST_CASE("worklist_range splits the worklist into contiguous, balanced ranges") {
    graph input = read_test_matrix();

    for (const int thread_count : {1, 2, 3, 4, 9, 12}) {
        CAPTURE(thread_count);
        const step_harness harness(input, thread_count, 1);

        int expected_begin = 0;
        const int chunk = (9 + thread_count - 1) / thread_count;
        for (const handshake_worker &worker : harness.m_workers) {
            const index_range range = worklist_range(&worker);
            CHECK(range.begin == expected_begin);
            CHECK(range.end == std::min(expected_begin + chunk, 9));
            CHECK(range.end - range.begin <= chunk);
            expected_begin = range.end;
        }
        CHECK(expected_begin == 9);
    }

    graph_deinit(&input);
}

TEST_CASE("one_way_extend_hand picks each vertex's strongest unmatched neighbor") {
    graph input = read_test_matrix();
    step_harness harness(input, 3, 1);

    harness.run(one_way_extend_hand);
    CHECK(harness.m_strongest == std::vector{3, 4, 5, 4, 1, 2, 3, 4, 7});

    // Pass 2: (1, 4) and (2, 5) are matched, so hands skip vertices 1, 2, 4, and 5.
    const int u = VERTEX_UNMATCHED;
    harness.m_partners = {u, 4, 5, u, 1, 2, u, u, u};
    harness.m_active = {0, 3, 6, 7, 8, -99, -99, -99, -99};
    for (handshake_worker &worker : harness.m_workers) {
        worker.active_count = 5;
    }
    harness.run(one_way_extend_hand);
    CHECK(harness.m_strongest[0] == 3);
    CHECK(harness.m_strongest[3] == 0);
    CHECK(harness.m_strongest[6] == 3);
    CHECK(harness.m_strongest[7] == 8);
    CHECK(harness.m_strongest[8] == 7);

    // Pass 3: every neighbor of vertex 6 is matched, so it has no hand to extend.
    harness.m_partners = {3, 4, 5, 0, 1, 2, u, 8, 7};
    harness.m_active[0] = 6;
    for (handshake_worker &worker : harness.m_workers) {
        worker.active_count = 1;
    }
    harness.run(one_way_extend_hand);
    CHECK(harness.m_strongest[6] == VERTEX_UNMATCHABLE);

    graph_deinit(&input);
}

TEST_CASE("one_way_check_handshakes matches mutual hands only") {
    graph input = read_test_matrix();
    step_harness harness(input, 2, 1);

    // Vertex 6 has no free neighbor; vertex 8 extends no hand this pass.
    const int u = VERTEX_UNMATCHED;
    harness.m_strongest = {3, 4, 5, 4, 1, 2, VERTEX_UNMATCHABLE, 4, u};
    harness.run(one_way_check_handshakes);

    CHECK(harness.m_partners == std::vector<int>{u, 4, 5, u, 1, 2, VERTEX_UNMATCHABLE, u, u});

    graph_deinit(&input);
}

TEST_CASE("worklist_count_unmatched and worklist_compact keep the unmatched vertices in order") {
    graph input = read_test_matrix();
    step_harness harness(input, 3, 1);

    const int u = VERTEX_UNMATCHED;
    harness.m_partners = {u, 4, 5, u, 1, 2, u, u, u};

    harness.run(worklist_count_unmatched);
    CHECK(harness.m_unmatched_counts == std::vector{1, 1, 3});

    harness.m_write_offsets = {0, 1, 2, 5};
    harness.run(worklist_compact);

    const std::vector next(harness.m_next_active.begin(), harness.m_next_active.begin() + 5);
    CHECK(next == std::vector{0, 3, 6, 7, 8});
    CHECK(harness.m_write_offsets[3] == 5);

    graph_deinit(&input);
}

TEST_CASE("n_way_find_candidates lists each vertex's N strongest unmatched neighbors") {
    graph input = read_test_matrix();
    step_harness harness(input, 4, 2);

    harness.run(n_way_find_candidates);

    CHECK(harness.m_candidate_counts == std::vector<int>(9, 2));
    CHECK(harness.m_candidates ==
          std::vector{3, 1, 4, 2, 5, 1, 4, 0, 1, 5, 2, 4, 3, 7, 4, 8, 7, 5});

    graph_deinit(&input);
}

TEST_CASE("n_way_extend_hand picks each vertex's strongest mutual candidate") {
    graph input = read_test_matrix();
    step_harness harness(input, 4, 2);

    harness.run(n_way_find_candidates);
    harness.run(n_way_extend_hand);

    // No candidate of vertex 6 chose it, so it waits for the next pass.
    CHECK(harness.m_strongest == std::vector<int>{3, 4, 5, 0, 1, 2, VERTEX_UNMATCHED, 8, 7});

    graph_deinit(&input);
}

TEST_CASE("handshake_match matches test.mtx the same way on any number of threads") {
    graph input = read_test_matrix();
    const std::vector<int> expected{3, 4, 5, 0, 1, 2, VERTEX_UNMATCHABLE, 8, 7};

    for (const int thread_count : {1, 2, 3, 4, 8, 16}) {
        CAPTURE(thread_count);
        CHECK(run_handshake(input, thread_count, 1) == expected);
        CHECK(run_handshake(input, thread_count, 2) == expected);
    }

    graph_deinit(&input);
}

TEST_CASE("handshake_match gives thread-count-independent results on a random graph") {
    const graph_matching::generator_options options{.vertices = 2000, .average_degree = 10};
    const auto edges = graph_matching::generate_edges(options);

    const graph_matching::testing::temporary_file file("graph_matching_assignment.mtx");
    {
        std::ofstream out(file.c_str());
        graph_matching::write_matrix_market(out, options, edges);
    }

    graph input{};
    REQUIRE(graph_init_from_matrix_market(&input, file.c_str()) == STATUS_OK);

    for (const int hand_count : {1, 2, 3}) {
        CAPTURE(hand_count);
        const std::vector one_thread = run_handshake(input, 1, hand_count);
        for (const int thread_count : {2, 4, 7}) {
            CAPTURE(thread_count);
            CHECK(run_handshake(input, thread_count, hand_count) == one_thread);
        }
    }

    graph_deinit(&input);
}

TEST_CASE("one-way handshaking gives exactly the greedy matching") {
    for (const int max_weight : {3, 1000}) {
        CAPTURE(max_weight);
        const graph_matching::generator_options options{
            .vertices = 1500, .average_degree = 8, .max_weight = max_weight, .seed = 7};
        const auto edges = graph_matching::generate_edges(options);

        const graph_matching::testing::temporary_file file("graph_matching_greedy.mtx");
        {
            std::ofstream out(file.c_str());
            graph_matching::write_matrix_market(out, options, edges);
        }

        graph input{};
        REQUIRE(graph_init_from_matrix_market(&input, file.c_str()) == STATUS_OK);

        matching greedy{};
        REQUIRE(matching_init(&greedy, input.vertex_count) == STATUS_OK);
        REQUIRE(matching_run_greedy(&greedy, &input) == STATUS_OK);
        const std::span expected(greedy.partners, static_cast<std::size_t>(greedy.vertex_count));

        CHECK(run_handshake(input, 4, 1) == std::vector<int>(expected.begin(), expected.end()));

        matching_deinit(&greedy);
        graph_deinit(&input);
    }
}
