#pragma once
#include <string>
#include <array>
#include <rv64/GPIntReg.hpp>
#include <set>

#include "Interpreter.hpp"

namespace rv64 {
    class Cpu {
    public:
        static constexpr size_t INT_REG_CNT = 32;

        explicit Cpu(VM &vm);
        Cpu(const Cpu &other);
        Cpu &operator=(Cpu &&other) noexcept;

        /// @brief Sets or removes a breakpoint at the specified line
        /// @param enable True to set breakpoint, false to remove
        /// @return True if operation succeeded, false otherwise
        bool set_breakpoint(size_t line, bool enable) noexcept;
        bool has_breakpoint(size_t line) const noexcept;
        void clear_breakpoints() noexcept;

        void set_pc(uint64_t new_pc);
        void move_pc(int64_t offset);
        [[nodiscard]] uint64_t get_pc() const;

        [[nodiscard]] GPIntReg &reg(int i) noexcept;
        [[nodiscard]] const GPIntReg &reg(int i) const noexcept;
        [[nodiscard]] GPIntReg &reg(Reg reg) noexcept;
        [[nodiscard]] const GPIntReg &reg(Reg reg) const noexcept;

        void print_cpu_state() const;

        /// @brief reads next instruction and updates the Cpu state
        /// @return false if reached the last instruction, true otherwise
        bool next_cycle();

        Interpreter m_interpreter;
    private:
        template<std::size_t... Is>
        static constexpr std::array<GPIntReg, sizeof...(Is)>
        reg_array_construct(std::index_sequence<Is...>) {
            return {GPIntReg(Is)...};
        }

    private:
        std::array<GPIntReg, INT_REG_CNT> m_int_regs;
        uint64_t m_pc = 0;
        VM &m_vm;

        std::array<uint64_t, INT_REG_CNT> m_int_regs_prev_vals = {};
        std::set<size_t> m_breakpoints{};
    };
}
