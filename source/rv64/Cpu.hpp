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

        explicit Cpu(VM &vm);

        void set_pc(uint64_t pc);

        void move_pc(int64_t offset);

        [[nodiscard]] GPIntReg &reg(int i) noexcept;
        [[nodiscard]] const GPIntReg &reg(int i) const noexcept;
        [[nodiscard]] GPIntReg &reg(Reg reg) noexcept;
        [[nodiscard]] const GPIntReg &reg(Reg reg) const noexcept;
        [[nodiscard]] uint64_t get_pc() const;

        [[nodiscard]] size_t get_current_line() const;

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
        size_t m_current_line = 0;

        std::array<uint64_t, INT_REG_CNT> m_int_regs_prev_vals = {};
    };
}
