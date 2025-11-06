#pragma once
#include <cstdint>
#include <string>
#include <array>
#include <rv64/GPIntReg.hpp>

#include "Interpreter.hpp"

namespace rv64 {
    class Cpu {
    public:
        static constexpr size_t INT_REG_CNT = 32;

        Cpu(VM &vm);

        void set_pc(uint64_t pc);

        [[nodiscard]] GPIntReg &get_reg(int i) noexcept;
        [[nodiscard]] const GPIntReg &get_int_reg(int i) const noexcept;
        [[nodiscard]] GPIntReg &get_int_reg(Reg reg) noexcept;
        [[nodiscard]] const GPIntReg &get_int_reg(Reg reg) const noexcept;
        [[nodiscard]] uint64_t get_pc() const;

        void print_cpu_state() const;

        /// @brief reads next instruction and updates the Cpu state
        /// @return false if reached the last instruction, true otherwise
        bool next_cycle();

    private:
        template<std::size_t... Is>
        static constexpr std::array<GPIntReg, sizeof...(Is)>
        reg_array_construct(std::index_sequence<Is...>) {
            return {GPIntReg(Is)...};
        }

    private:
        std::array<GPIntReg, INT_REG_CNT> m_int_regs;
        uint64_t m_pc = 0;
        Interpreter m_interpreter;
        VM &m_vm;
    };
}
