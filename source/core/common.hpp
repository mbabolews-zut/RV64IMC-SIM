#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <ui.hpp>
#include <intN.hpp>

using namespace std::literals;

using int5 = intN<5>;
using int6 = intN<6>;
using int8 = intN<8>;
using int11 = intN<11>;
using int12 = intN<12>;
using int20 = intN<20>;
using uint5 = uintN<5>;
using uint6 = uintN<6>;
using uint8 = uintN<8>;
using uint12 = uintN<12>;
using uint20 = uintN<20>;

inline std::string to_lowercase(std::string_view str) {
    std::string result;
    std::ranges::transform(str, std::back_inserter(result), ::tolower);
    return result;
}

// Helpers for instantiating templates for all integer types
// Use like this:
// MY_TEMPLATE_INSTANTIATION_MACRO(type) { template ... }
// FOR_EACH_INT(MY_TEMPLATE_INSTANTIATION_MACRO)
#define ALL_INT_TYPES \
uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t

#define FOR_EACH_INT(MACRO) \
MACRO(uint8_t) MACRO(uint16_t) MACRO(uint32_t) MACRO(uint64_t) \
MACRO(int8_t) MACRO(int16_t) MACRO(int32_t) MACRO(int64_t)

