#pragma once
#include <bit>
#include <concepts>
#include <cstdint>
#include <type_traits>

namespace endianness {

// ============================================================================
// Low-level byte swap functions
// ============================================================================

namespace detail {

// Try C++23 std::byteswap first
#if defined(__cpp_lib_byteswap) && __cpp_lib_byteswap >= 202110L

    inline constexpr uint16_t bswap16(uint16_t x) noexcept {
        return std::byteswap(x);
    }

    inline constexpr uint32_t bswap32(uint32_t x) noexcept {
        return std::byteswap(x);
    }

    inline constexpr uint64_t bswap64(uint64_t x) noexcept {
        return std::byteswap(x);
    }

// Compiler builtins (GCC, Clang)
#elif defined(__GNUC__) || defined(__clang__)

    inline uint16_t bswap16(uint16_t x) noexcept {
        return __builtin_bswap16(x);
    }

    inline uint32_t bswap32(uint32_t x) noexcept {
        return __builtin_bswap32(x);
    }

    inline uint64_t bswap64(uint64_t x) noexcept {
        return __builtin_bswap64(x);
    }

// MSVC intrinsics
#elif defined(_MSC_VER)

    #include <cstdlib>

    inline uint16_t bswap16(uint16_t x) noexcept {
        return _byteswap_ushort(x);
    }

    inline uint32_t bswap32(uint32_t x) noexcept {
        return _byteswap_ulong(x);
    }

    inline uint64_t bswap64(uint64_t x) noexcept {
        return _byteswap_uint64(x);
    }

// Portable fallback
#else

    inline constexpr uint16_t bswap16(uint16_t x) noexcept {
        return ((x >> 8) & 0xFF) | ((x & 0xFF) << 8);
    }

    inline constexpr uint32_t bswap32(uint32_t x) noexcept {
        return ((x & UINT32_C(0xFF000000)) >> 24)
             | ((x & UINT32_C(0x00FF0000)) >> 8)
             | ((x & UINT32_C(0x0000FF00)) << 8)
             | ((x & UINT32_C(0x000000FF)) << 24);
    }

    inline constexpr uint64_t bswap64(uint64_t x) noexcept {
        return ((x & UINT64_C(0xFF00000000000000)) >> 56)
             | ((x & UINT64_C(0x00FF000000000000)) >> 40)
             | ((x & UINT64_C(0x0000FF0000000000)) >> 24)
             | ((x & UINT64_C(0x000000FF00000000)) >> 8)
             | ((x & UINT64_C(0x00000000FF000000)) << 8)
             | ((x & UINT64_C(0x0000000000FF0000)) << 24)
             | ((x & UINT64_C(0x000000000000FF00)) << 40)
             | ((x & UINT64_C(0x00000000000000FF)) << 56);
    }

#endif

} // namespace detail

// ============================================================================
// Generic byte swap
// ============================================================================

/// Swap byte order of any integral type
template<std::integral T>
constexpr T swap_endian(T value) noexcept {
    if constexpr (sizeof(T) == 1) {
        return value;  // No swap for single byte
    }
    else if constexpr (sizeof(T) == 2) {
        return std::bit_cast<T>(
            detail::bswap16(std::bit_cast<uint16_t>(value))
        );
    }
    else if constexpr (sizeof(T) == 4) {
        return std::bit_cast<T>(
            detail::bswap32(std::bit_cast<uint32_t>(value))
        );
    }
    else if constexpr (sizeof(T) == 8) {
        return std::bit_cast<T>(
            detail::bswap64(std::bit_cast<uint64_t>(value))
        );
    }
    else {
        static_assert(sizeof(T) <= 8, "Unsupported integer size");
        return value;
    }
}

// ============================================================================
// Platform-aware conversions
// ============================================================================

/// Swap bytes if running on little-endian platform
/// (i.e., convert to/from big-endian)
template<std::integral T>
constexpr T swap_on_le_platform(T value) noexcept {
    if constexpr (std::endian::native == std::endian::little) {
        return swap_endian(value);
    }
    return value;
}

/// Swap bytes if running on big-endian platform
/// (i.e., convert to/from little-endian)
template<std::integral T>
constexpr T swap_on_be_platform(T value) noexcept {
    if constexpr (std::endian::native == std::endian::big) {
        return swap_endian(value);
    }
    return value;
}

}
