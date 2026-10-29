///
/// \file       ring_buffer.hpp
/// \brief      Fixed-capacity FIFO ring buffer.
///
/// \details    Demonstrates a header-only class template whose whole
///             interface is usable in constant expressions.
///
/// \author     galudino
/// \date       2026-10-24
///

#ifndef STARTER_RING_BUFFER_HPP
#define STARTER_RING_BUFFER_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <optional>
#include <utility>

namespace starter {

///
/// \brief      Fixed-capacity first-in, first-out queue.
///
/// \details    Storage is inline; the buffer never allocates. Pushing
///             to a full buffer is rejected rather than overwriting.
///
/// \tparam     T           Element type.
/// \tparam     Capacity    Maximum number of elements held at once.
///
template <std::semiregular T, std::size_t Capacity>
    requires(Capacity > 0)
class ring_buffer {
public:
    using value_type = T;          ///< Element type.
    using size_type = std::size_t; ///< Unsigned type for sizes and indices.

    ///
    /// \brief      Append an element to the back of the buffer.
    ///
    /// \param      value       Element to append.
    ///
    /// \return     True if the element was stored; false if the buffer was full.
    ///
    [[nodiscard]] constexpr bool push(T value) {
        if (full()) {
            return false;
        }

        m_storage[(m_head + m_size) % Capacity] = std::move(value);
        ++m_size;

        return true;
    }

    ///
    /// \brief      Remove the element at the front of the buffer.
    ///
    /// \return     The removed element, or std::nullopt if the buffer was empty.
    ///
    [[nodiscard]] constexpr std::optional<T> pop() {
        if (empty()) {
            return std::nullopt;
        }

        auto value = std::move(m_storage[m_head]);
        m_head = (m_head + 1) % Capacity;
        --m_size;

        return value;
    }

    ///
    /// \brief      Remove every element.
    ///
    constexpr void clear() noexcept {
        m_head = 0;
        m_size = 0;
    }

    ///
    /// \return     Number of elements currently stored.
    ///
    [[nodiscard]] constexpr size_type size() const noexcept { return m_size; }

    ///
    /// \return     Maximum number of elements the buffer can hold.
    ///
    [[nodiscard]] static constexpr size_type capacity() noexcept { return Capacity; }

    ///
    /// \return     True if the buffer holds no elements.
    ///
    [[nodiscard]] constexpr bool empty() const noexcept { return m_size == 0; }

    ///
    /// \return     True if the buffer holds Capacity elements.
    ///
    [[nodiscard]] constexpr bool full() const noexcept { return m_size == Capacity; }

private:
    std::array<T, Capacity> m_storage{}; ///< Inline element storage.
    size_type m_head = 0;                ///< Index of the front element.
    size_type m_size = 0;                ///< Number of stored elements.
};

} // namespace starter

#endif // STARTER_RING_BUFFER_HPP
