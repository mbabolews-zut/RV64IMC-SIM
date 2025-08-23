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
        std::cerr << "[ERROR] " << msg << std::endl;
    }
    inline void print_output(std::string_view msg) {
        std::cout << "[OUTPUT] " << msg << std::endl;
    }
    inline void print_warning(std::string_view msg) {
        std::cout << "[WARNING] " << msg << std::endl;
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
    constexpr intN(int64_t v) : m_val(0) { // NOLINT(*-explicit-constructor)
        m_val = (v & pow_of_2(NBits - 1)) ? v : -1;
        if (m_val == v) return;
        m_val <<= NBits;
        m_val |= v;
    }

    constexpr operator int64_t() const { // NOLINT(*-explicit-constructor)
        return m_val;
    }

    constexpr uint64_t raw() {
        return m_val & (pow_of_2(NBits) - 1);
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
