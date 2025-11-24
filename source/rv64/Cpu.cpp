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

        auto fetch = m_vm.m_memory.get_instruction_at(get_pc(), mem_err);
        if (mem_err != MemErr::None) {
            if (mem_err != MemErr::ProgramExit) {
                ui::print_error(Memory::err_to_string(mem_err));
                m_vm.error_stop();
            }
            return false;
        }

        m_pc += fetch.inst.byte_size();
        m_interpreter.exec_instruction(fetch.inst);
        m_current_line = m_vm.m_memory.get_instruction_at(get_pc(), mem_err).lineno;
        return m_pc < m_vm.m_memory.get_instruction_end_addr();
    }

    Cpu::Cpu(VM &vm)
        : m_int_regs(reg_array_construct(std::make_index_sequence<INT_REG_CNT>{})),
          m_interpreter(vm), m_vm(vm) {
    }
}
