#pragma once
#include <cstdint>
#include <bit>
#include <concepts>

namespace helper {
    template<std::integral T = int64_t>
    constexpr T _2sqr_n(int64_t n) {
        return T(1) << n;
    }
}

template<size_t NBits>
struct intN {
    static_assert(NBits < 64);

    constexpr intN(int64_t v); // NOLINT(*-explicit-constructor)

    constexpr operator int64_t() const; // NOLINT(*-explicit-constructor)

    constexpr explicit operator int32_t() const;

    [[nodiscard]] constexpr int32_t as_i32() const;

    [[nodiscard]] constexpr uint64_t zero_extended() const;

    static constexpr int64_t MAX = helper::_2sqr_n(NBits - 1) - 1;
    static constexpr int64_t MIN = -helper::_2sqr_n(NBits - 1);

private:
    int64_t m_val;
};

template<size_t NBits>
constexpr intN<NBits>::intN(int64_t v): m_val(v) {
    if (!(v & helper::_2sqr_n(NBits - 1))) return;
    m_val = INT64_C(-1) << NBits;
    m_val |= v;
}

template<size_t NBits>
constexpr intN<NBits>::operator int64_t() const {
    return std::bit_cast<int64_t>(m_val);
}

template<size_t NBits>
constexpr intN<NBits>::operator int32_t() const {
    return std::bit_cast<int64_t>(m_val);
}

template<size_t NBits>
constexpr int32_t intN<NBits>::as_i32() const {
    return std::bit_cast<int64_t>(m_val);
}

template<size_t NBits>
constexpr uint64_t intN<NBits>::zero_extended() const {
    return m_val & (helper::_2sqr_n(NBits) - 1);
}

template<size_t NBits>
struct uintN {
    static_assert(NBits < 64);

    uintN(uint64_t v) : m_val(v) { // NOLINT(*-explicit-constructor)
        m_val &= (helper::_2sqr_n(NBits) - 1);
    }

    constexpr operator uint64_t() const { // NOLINT(*-explicit-constructor)
        return m_val & (helper::_2sqr_n(NBits) - 1);
    }

    static constexpr uint64_t MAX = helper::_2sqr_n(NBits) - 1;
    static constexpr uint64_t MIN = 0;

private:
    uint64_t m_val = 0;
};
