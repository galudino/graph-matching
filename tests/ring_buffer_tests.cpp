///
/// \file       ring_buffer_tests.cpp
/// \brief      Unit tests for the ring buffer library.
///
/// \author     galudino
/// \date       2026-10-24
///

#include <starter/ring_buffer.hpp>

#include <doctest/doctest.h>

#include <string>

namespace {

///
/// \brief      Push past capacity, then drain, entirely at compile time.
///
/// \return     True if the buffer keeps FIFO order and rejects overflow.
///
consteval bool fifo_order_holds() {
    starter::ring_buffer<int, 3> buffer;

    const bool accepted = buffer.push(1) && buffer.push(2) && buffer.push(3);
    const bool rejected = !buffer.push(4);

    return accepted && rejected && buffer.pop() == 1 && buffer.pop() == 2 && buffer.pop() == 3 &&
           !buffer.pop().has_value();
}

static_assert(fifo_order_holds());
static_assert(starter::ring_buffer<int, 8>::capacity() == 8);

} // namespace

TEST_CASE("ring_buffer starts empty") {
    starter::ring_buffer<int, 4> buffer;

    CHECK(buffer.empty());
    CHECK_FALSE(buffer.full());
    CHECK(buffer.size() == 0);
    CHECK_FALSE(buffer.pop().has_value());
}

TEST_CASE("ring_buffer rejects pushes when full") {
    starter::ring_buffer<int, 2> buffer;

    CHECK(buffer.push(1));
    CHECK(buffer.push(2));
    CHECK(buffer.full());
    CHECK_FALSE(buffer.push(3));
    CHECK(buffer.size() == 2);
}

TEST_CASE("ring_buffer wraps around its storage") {
    starter::ring_buffer<int, 3> buffer;

    // Advance the head so later pushes wrap past the end of the array.
    for (int round = 0; round < 5; ++round) {
        REQUIRE(buffer.push(round));
        REQUIRE(buffer.push(round + 100));
        CHECK(buffer.pop() == round);
        CHECK(buffer.pop() == round + 100);
    }

    CHECK(buffer.empty());
}

TEST_CASE("ring_buffer moves non-trivial element types") {
    starter::ring_buffer<std::string, 2> buffer;

    CHECK(buffer.push("first"));
    CHECK(buffer.push(std::string(64, 'x')));
    CHECK(buffer.pop() == "first");
    CHECK(buffer.pop() == std::string(64, 'x'));
}

TEST_CASE("ring_buffer clear discards every element") {
    starter::ring_buffer<int, 3> buffer;

    CHECK(buffer.push(1));
    CHECK(buffer.push(2));
    buffer.clear();

    CHECK(buffer.empty());
    CHECK(buffer.push(3));
    CHECK(buffer.pop() == 3);
}
