#pragma once
#include <cstdint>
#include <string>
#include <rv64/Reg.hpp>
#include <variant>

namespace rv64 {
    class GPIntReg : public Reg {
    public:
        [[nodiscard]] uint64_t val() const;
        [[nodiscard]] uint64_t &val();
        [[nodiscard]] int64_t sval() const;
        [[nodiscard]] int64_t &sval();
        [[nodiscard]] int32_t as_i32() const;
        [[nodiscard]] uint32_t as_u32() const;

        GPIntReg &operator=(uint64_t val);
        GPIntReg &operator=(int64_t val);
        GPIntReg &operator=(int32_t val);

        constexpr auto operator==(std::signed_integral auto other) const;

        constexpr auto operator==(std::unsigned_integral auto other) const;

    private:
        explicit GPIntReg(int idx) : Reg(idx) {}
        std::variant<uint64_t, int64_t> m_value = 0;
        friend class Cpu; // only Cpu can construct general purpose registers
    };

    constexpr auto GPIntReg::operator==(std::signed_integral auto other) const {
        return sval() == static_cast<int64_t>(other);
    }

    constexpr auto GPIntReg::operator==(std::unsigned_integral auto other) const {
        return val() == static_cast<uint64_t>(other);
    }
}
