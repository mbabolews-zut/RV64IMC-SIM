#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <rv64/IntReg.hpp>

namespace rv64 {


    class Cpu {
    public:
        static constexpr size_t INT_REG_CNT = 32;

        void set_pc(uint64_t pc);

        [[nodiscard]] IntReg &get_int_reg(size_t i) noexcept;
        [[nodiscard]] const IntReg &get_int_reg(size_t i) const noexcept;
        [[nodiscard]] uint64_t get_pc() const;

        Cpu();

    private:
        template<class T, std::size_t... Is>
        static constexpr std::array<T, sizeof...(Is)>
        reg_array_construct(std::index_sequence<Is...>) {
            return {IntReg(Is, true)...};
        }


        std::array<IntReg, 32> m_int_regs;
        uint64_t m_pc = 0;
    };
}
