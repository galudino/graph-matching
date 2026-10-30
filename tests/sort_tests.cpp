///
/// \file       sort_tests.cpp
/// \brief      Unit tests for the coordinate-list sorts.
///
/// \author     Gemuele (Gem) Aludino
/// \date       2026-10-29
///

#include <graph_matching/sort.h>

#include <doctest/doctest.h>

#include <array>

namespace {

///
/// \brief      Entries with ties in every key, in no particular order.
///
struct entries {
    std::array<int, 6> row{1, 0, 1, 0, 1, 0};
    std::array<int, 6> col{2, 3, 0, 1, 2, 3};
    std::array<double, 6> val{1.0, 2.0, 5.0, 2.0, 3.0, 4.0};
};

} // namespace

TEST_CASE("sort_rcw orders by row, column, then descending weight") {
    entries e;

    sort_rcw(e.row.data(), e.col.data(), e.val.data(), static_cast<int>(e.row.size()));

    CHECK(e.row == std::array{0, 0, 0, 1, 1, 1});
    CHECK(e.col == std::array{1, 3, 3, 0, 2, 2});
    CHECK(e.val == std::array{2.0, 4.0, 2.0, 5.0, 3.0, 1.0});
}

TEST_CASE("sort_rwc orders by row, descending weight, then column") {
    entries e;

    sort_rwc(e.row.data(), e.col.data(), e.val.data(), static_cast<int>(e.row.size()));

    CHECK(e.row == std::array{0, 0, 0, 1, 1, 1});
    CHECK(e.val == std::array{4.0, 2.0, 2.0, 5.0, 3.0, 1.0});
    CHECK(e.col == std::array{3, 1, 3, 0, 2, 2});
}

TEST_CASE("sort_wrc orders by descending weight, row, then column") {
    entries e;

    sort_wrc(e.row.data(), e.col.data(), e.val.data(), static_cast<int>(e.row.size()));

    CHECK(e.val == std::array{5.0, 4.0, 3.0, 2.0, 2.0, 1.0});
    CHECK(e.row == std::array{1, 0, 1, 0, 0, 1});
    CHECK(e.col == std::array{0, 3, 2, 1, 3, 2});
}

TEST_CASE("sorts accept empty and single-entry input") {
    std::array row{7};
    std::array col{8};
    std::array val{9.0};

    sort_rcw(row.data(), col.data(), val.data(), 0);
    sort_rwc(row.data(), col.data(), val.data(), 1);
    sort_wrc(row.data(), col.data(), val.data(), 1);

    CHECK(row[0] == 7);
    CHECK(col[0] == 8);
    CHECK(val[0] == 9.0);
}
