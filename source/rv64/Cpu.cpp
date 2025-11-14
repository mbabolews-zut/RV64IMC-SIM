#include "Cpu.hpp"
#include <cassert>
#include <format>

#include "VM.hpp"

namespace rv64 {
    uint64_t Cpu::get_pc() const {
        assert(m_pc % 2 == 0);
        return m_pc;
    }

    size_t Cpu::get_current_line() const {
        return m_current_line;
    }

    void Cpu::print_cpu_state() const {
        std::cout << std::format("pc = 0x{:<16X}\n", get_pc());
        for (size_t i = 0; i < m_int_regs.size(); i++) {
            const auto &reg = m_int_regs[i];
            std::cout << std::format("{}[{:4} {:>5} = 0x{:016X} (i64:{:<20}\033[0m",
                                     reg.val() != m_int_regs_prev_vals[i] ? "\033[0;31m" : "", // red if changed
                                     reg.get_name() + "]",
                                     reg.get_sym_name(),
                                     reg.val(),
                                     std::to_string(reg.sval()) + ')');
            std::cout << (i % 2 == 0 ? " " : "\n");
        }
    }

    bool Cpu::next_cycle() {
        MemErr mem_err;
        // preserve previous reg states
        for (int i = 0; i < m_int_regs.size(); i++)
            m_int_regs_prev_vals[i] = m_int_regs[i].val();

        auto instruction = m_vm.m_memory.get_instruction_at(get_pc(), mem_err, &m_current_line);
        if (mem_err != MemErr::None) {
            if (mem_err != MemErr::ProgramExit) {
                PRINT_ERROR(Memory::err_to_string(mem_err));
                m_vm.error_stop();
            }
            return false;
        }
        m_pc += instruction.byte_size(); // advance PC
        m_interpreter.exec_instruction(instruction);
        return m_vm.m_memory.get_instruction_end_addr() < m_pc;
    }

    Cpu::Cpu(VM &vm)
        : m_int_regs(reg_array_construct(std::make_index_sequence<INT_REG_CNT>{})),
          m_interpreter(vm), m_vm(vm) {
    }
}
