#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string_view>
#include <type_traits>

/// TODO: move somewhere else

namespace gui {
    inline void display_error(std::string_view msg) {
        std::cerr << msg << std::endl;
    }
}

template<typename T = int64_t>
constexpr T pow_of_2(int64_t n) {
    static_assert(std::is_integral_v<T>, "T must be an integral type");
    T result = 1;
    return result << n;
}

template<size_t NBits>
struct intN {
    constexpr intN(int64_t v) : m_val(v & (pow_of_2(NBits - 1) - 1)) { // NOLINT(*-explicit-constructor)
        m_val = (v & pow_of_2(NBits - 1)) ? m_val : m_val * -1;
    }

    constexpr operator int64_t() const { // NOLINT(*-explicit-constructor)
        return m_val;
    }

    constexpr uint64_t raw() {
        return std::abs(m_val) | (m_val < 0 ? pow_of_2(NBits - 1) : 0);
    }

private:
    int64_t m_val;
};

using int5 = intN<5>;
using int12 = intN<12>;
using int20 = intN<20>;
using uint5 = uint64_t;
using uint12 = uint64_t;
using uint20 = uint64_t;
