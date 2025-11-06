#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string_view>
#include <type_traits>
using namespace std::literals;

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

    inline void print_info(std::string_view msg) {
        std::cout << "[INFO] " << msg << std::endl;
    }
}

inline std::string to_lowercase(std::string_view str) {
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result), ::tolower);
    return result;
}

template<std::integral T = int64_t>
constexpr T pow_of_2(int64_t n) {
    T result = 1;
    return result << n;
}

template<size_t NBits>
struct intN {
    static_assert(NBits < 64);

    constexpr intN(int64_t v) : m_val(v) { // NOLINT(*-explicit-constructor)
        if (!(v & pow_of_2(NBits - 1))) return;
        m_val = INT64_C(-1) << NBits;
        m_val |= v;
    }

    constexpr operator int64_t() const { // NOLINT(*-explicit-constructor)
        return m_val;
    }

    [[nodiscard]] constexpr uint64_t raw() const {
        return m_val & (pow_of_2(NBits) - 1);
    }

    static constexpr uint64_t MAX = pow_of_2(NBits - 1) - 1;
    static constexpr int64_t MIN = -pow_of_2(NBits - 1);

private:
    int64_t m_val;
};

template<size_t NBits>
struct uintN {
    static_assert(NBits < 64);

    uintN(uint64_t v) : m_val(v) { // NOLINT(*-explicit-constructor)
        m_val &= (pow_of_2(NBits) - 1);
    }

    constexpr operator uint64_t() const { // NOLINT(*-explicit-constructor)
        return m_val & (pow_of_2(NBits) - 1);
    }

    static constexpr uint64_t MAX = pow_of_2(NBits) - 1;
    static constexpr uint64_t MIN = 0;

private:
    uint64_t m_val = 0;
};

using int5 = intN<5>;
using int12 = intN<12>;
using int20 = intN<20>;
using uint5 = uintN<5>;
using uint6 = uintN<6>;
using uint12 = uintN<12>;
using uint20 = uintN<20>;

// Helpers for instantiating templates for all integer types
// Use like this:
// MY_TEMPLATE_INSTANTIATION_MACRO(type) { template ... }
// FOR_EACH_INT(MY_TEMPLATE_INSTANTIATION_MACRO)
#define ALL_INT_TYPES \
uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t

#define FOR_EACH_INT(MACRO) \
MACRO(uint8_t) MACRO(uint16_t) MACRO(uint32_t) MACRO(uint64_t) \
MACRO(int8_t) MACRO(int16_t) MACRO(int32_t) MACRO(int64_t)
