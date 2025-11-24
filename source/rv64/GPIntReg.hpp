#pragma once
#include <cstdint>
#include <string>
#include <rv64/Reg.hpp>
#include <compare>

namespace rv64 {
    class GPIntReg : public Reg {
    public:
        [[nodiscard]] uint64_t val() const noexcept { return m_storage.u; }
        [[nodiscard]] uint64_t &val() noexcept { return m_storage.u; }
        [[nodiscard]] int64_t sval() const noexcept { return m_storage.s; }
        [[nodiscard]] int64_t &sval() noexcept { return m_storage.s; }
        void set_sval(int64_t v) noexcept { m_storage.s = v; }
        [[nodiscard]] int32_t as_i32() const noexcept { return static_cast<int32_t>(m_storage.u); }
        [[nodiscard]] uint32_t as_u32() const noexcept { return static_cast<uint32_t>(m_storage.u); }

        GPIntReg &operator=(std::signed_integral auto v) noexcept {
            if (idx() == 0) return *this; // writes to x0 are ignored
            m_storage.s = static_cast<int64_t>(v);
            return *this;
        }

        GPIntReg &operator=(std::unsigned_integral auto v) noexcept {
            if (idx() == 0) return *this; // writes to x0 are ignored
            m_storage.u = static_cast<uint64_t>(v);
            return *this;
        }

        // explicit signed comparison (avoid ambiguous overloads for integer literals)
        bool operator==(int64_t other) const noexcept { return sval() == other; }
        std::strong_ordering operator<=>(int64_t other) const noexcept { return sval() <=> other; }

    private:
        explicit GPIntReg(int idx) : Reg(idx) {}

        union {
            uint64_t u;
            int64_t s;
        } m_storage{};

        friend class Cpu; // only Cpu can construct general purpose registers
    };
}
